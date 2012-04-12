#include "partialsumSSA.hh"
#include "openmp.hh"

namespace Fluc {
namespace Models {

PartialSumSSA::PartialSumSSA(libsbml::Model *model, int size, int seed, size_t opt_level,size_t num_threads) :
    StochasticSimulator(model, size, seed,num_threads),
    ConstantStoichiometryMixin((BaseModel &)(*this)),
    byte_code(this->numReactions()),
    sparseStoichiometry(numSpecies(),numReactions()),
    prop(this->numThreads(), Eigen::VectorXd::Zero(this->numReactions()) ),
    interpreter(this->numThreads()),
    mSize(ceil(sqrt(numReactions()))),
    partialSums(this->numThreads(), Eigen::VectorXd::Zero(this->mSize)),
    priorityList(mSize,mSize),
    groupList(mSize*mSize),
    dependencyGraph(numReactions(),numReactions())
{

    Intprt::Compiler<Eigen::VectorXd> compiler(this->stateIndex);

    // setup the interpreter from a dependency graph
    int d;
    for (size_t j=0;j<this->numReactions();j++)
    {
        this->dependencyGraph.startVec(j);
        for (size_t i=0;i<this->numReactions();i++)
        {
            d=0;
            for(size_t k=0;k<this->numSpecies();k++)
                // ask if species k is changed by reaction j
                // which affects the propensities of reaction i in which species k is a modifier or a reactant
                d = d || ( (this->reactants_stoichiometry(k,i)!=0 || this->propensities[i].has(this->species[k]) ) && this->stoichiometry(k,j)!=0 ) ;

            if (d!=0)
            {
                compiler.compileExpressionAndStore(this->propensities[i],i);
                this->dependencyGraph.insertBack(i,j) = d;
            }
        }

        // optimize and store bytecode
        compiler.optimize(opt_level);
        this->byte_code[j] = compiler.getCode();
        //reset compiler
        compiler.reset();

    }
    this->dependencyGraph.finalize();

    // compile all propensities for byte code evaluation
    for(size_t i=0; i<this->numReactions(); i++)
        compiler.compileExpressionAndStore(this->propensities[i],i);

    // optimize and store
    compiler.optimize(opt_level);
    this->all_byte_code = compiler.getCode();

    // fill sparse stoichiometry
    for(size_t j=0; j<this->numReactions(); ++j)
    {
      this->sparseStoichiometry.startVec(j);
      for(size_t i=0; i<this->numSpecies(); i++)
          if (this->stoichiometry(i,j)!=0) this->sparseStoichiometry.insertBack(i,j) = this->stoichiometry(i,j);
    }
    this->sparseStoichiometry.finalize();

    // make priority list
    size_t k = 0;
    for(size_t i=0; i<mSize; i++){
    for(size_t j=0; j<mSize; j++){
        this->priorityList(i,j) = k;
        this->groupList(k++)=j;
    }}
}

size_t
PartialSumSSA::linSearch(const double &propensitySum)
{

    // select reaction
    double r = this->rand[OpenMP::getThreadNum()].rand()*propensitySum;
    double sum = prop[OpenMP::getThreadNum()](0);

    size_t reaction = 0;
    while(sum < r) sum += prop[OpenMP::getThreadNum()](++reaction);

    return reaction;

}

void
PartialSumSSA::run(double step)
{

      // initialization
      double propensitySum;	        // sum of propensities
      double t,tau;			// time between reactions
      size_t group;                     // group number selected
      size_t reaction;			// reaction number selected

     for(int sid=0;sid<this->ensembleSize;sid++)
      {
          t=0;

          interpreter[OpenMP::getThreadNum()].setCode(&all_byte_code);
          interpreter[OpenMP::getThreadNum()].run(this->observationMatrix.row(sid), prop[OpenMP::getThreadNum()]);

          // initialize sum propensities
          for(size_t i=0;i<mSize;i++){
              partialSums[OpenMP::getThreadNum()](i)=0;
              for(size_t j=0;j<mSize;j++)
              {
                  if(this->priorityList(j,i)>=int(numReactions())) break;
                  partialSums[OpenMP::getThreadNum()](i)
                          += prop[OpenMP::getThreadNum()](this->priorityList(j,i));
              }
          }

          propensitySum = partialSums[OpenMP::getThreadNum()].sum();

          while(t < step)
          {

              // sample tau
              if(propensitySum > 0)
              {
                  tau = -std::log(this->rand[OpenMP::getThreadNum()].rand()) / propensitySum;
              }
              else
              {
                  break;
              }

              // select group
              group = 0;
              double r = this->rand[OpenMP::getThreadNum()].rand()*propensitySum;
              double sum = this->partialSums[OpenMP::getThreadNum()](0);
              while(sum < r) sum += this->partialSums[OpenMP::getThreadNum()](++group);

              // select reaction
              reaction = 0;
              sum -= partialSums[OpenMP::getThreadNum()](group);
              sum += this->prop[OpenMP::getThreadNum()](this->priorityList(0,group));
              while(sum < r) sum+= this->prop[OpenMP::getThreadNum()](this->priorityList(++reaction,group));

              reaction = this->priorityList(reaction,group);

              // update population of chemical species
              for (Eigen::SparseMatrix<double>::InnerIterator it(this->sparseStoichiometry,reaction); it; ++it)
              {
                  this->observationMatrix(sid,it.row())+=it.value();
              }

              // update partial sums (i): substract old values
              for (Eigen::SparseMatrix<int>::InnerIterator it(this->dependencyGraph,reaction); it; ++it)
              {
                   partialSums[OpenMP::getThreadNum()](groupList(it.row()))-=prop[OpenMP::getThreadNum()](it.row());
                   propensitySum -= prop[OpenMP::getThreadNum()](it.row());
              }

              // update only propensities that are changed in reaction
              interpreter[OpenMP::getThreadNum()].setCode(&(byte_code[reaction]));
              interpreter[OpenMP::getThreadNum()].run(this->observationMatrix.row(sid),prop[OpenMP::getThreadNum()]);

              // update partial sums (ii): add new values
              for (Eigen::SparseMatrix<int>::InnerIterator it(this->dependencyGraph,reaction); it; ++it)
              {
                   partialSums[OpenMP::getThreadNum()](groupList(it.row())) += prop[OpenMP::getThreadNum()](it.row());
                   propensitySum += prop[OpenMP::getThreadNum()](it.row());
              }

              // update time
              t += tau;

          } //end time step loop

      } // end ensemble loop

}

}
}
