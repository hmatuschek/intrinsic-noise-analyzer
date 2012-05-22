#include "optimizedSSA.hh"
#include "openmp.hh"

namespace Fluc {
namespace Models {

OptimizedSSA::OptimizedSSA(libsbml::Model *model, int size, int seed, size_t opt_level,size_t num_threads) :
    StochasticSimulator(model, size, seed,num_threads),
    ConstantStoichiometryMixin((BaseModel &)(*this)),
    byte_code(this->numReactions()),
    sparseStoichiometry(numSpecies(),numReactions()),
    prop( this->numThreads(), Eigen::VectorXd::Zero(this->numReactions()) ),
    interpreter( this->numThreads() )
{

    Evaluate::bci::Compiler<Eigen::VectorXd> compiler(this->stateIndex);

    // setup the interpreter from a dependency graph
    int d;
    for (size_t j=0;j<this->numReactions();j++)
    {
        // Set byte-code to compile into:
        compiler.setCode(&byte_code[j]);

        for (size_t i=0;i<this->numReactions();i++)
        {
            d=0;
            for(size_t k=0;k<this->numSpecies();k++) {
                // ask if species k is changed by reaction j
                // which affects the propensities of reaction i in which species k is a modifier or a reactant
                d = d || ( (this->reactants_stoichiometry(k,i)!=0 || this->propensities[i].has(this->species[k]) ) && this->stoichiometry(k,j)!=0 ) ;
            }

            if (d!=0){
                compiler.compileExpressionAndStore(this->propensities[i],i);
            }
        }

        // optimize bytecode
        compiler.finalize(opt_level);
    }

    compiler.setCode(&all_byte_code);
    // and compile propensities for byte code evaluation
    for(size_t i=0; i<this->numReactions(); i++)
        compiler.compileExpressionAndStore(this->propensities[i],i);

    // optimize and store
    compiler.finalize(opt_level);

    // fill sparse stoichiometry
    for(size_t j=0; j<this->numReactions(); ++j)
    {
      this->sparseStoichiometry.startVec(j);
      for(size_t i=0; i<this->numSpecies(); i++)
          if (this->stoichiometry(i,j)!=0) this->sparseStoichiometry.insertBack(i,j) = this->stoichiometry(i,j);
    }
    this->sparseStoichiometry.finalize();

}

void
OptimizedSSA::run(double step)
{

      // initialization
      double propensitySum;	        // sum of propensities
      double t,tau;			// time between reactions
      size_t reaction;			// reaction number selected

#pragma omp parallel for if(this->numThreads()>1) num_threads(this->numThreads()) schedule(dynamic) private(propensitySum,tau,t,reaction)
      for(int sid=0;sid<this->ensembleSize;sid++)
      {
          t=0;

          interpreter[OpenMP::getThreadNum()].setCode(&all_byte_code);
          interpreter[OpenMP::getThreadNum()].run(this->observationMatrix.row(sid), prop[OpenMP::getThreadNum()]);

          // initialize sum propensities
          propensitySum = prop[OpenMP::getThreadNum()].sum();

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

              // select reaction

              double r = this->rand[OpenMP::getThreadNum()].rand()*propensitySum;
              double sum = prop[OpenMP::getThreadNum()](0);

              reaction = 0;
              while(sum < r) sum += prop[OpenMP::getThreadNum()](++reaction);

              // update population of chemical species
              for (Eigen::SparseMatrix<double>::InnerIterator it(this->sparseStoichiometry,reaction); it; ++it)
              {
                  this->observationMatrix(sid,it.row())+=it.value();
              }

              // update only propensities that are changed in reaction
              interpreter[OpenMP::getThreadNum()].setCode(&(byte_code[reaction]));
              interpreter[OpenMP::getThreadNum()].run(this->observationMatrix.row(sid),prop[OpenMP::getThreadNum()]);

              propensitySum = prop[OpenMP::getThreadNum()].sum();

              // update time
              t += tau;

          } //end time step loop

      } // end ensemble loop

}

}
}
