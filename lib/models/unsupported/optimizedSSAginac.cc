#include "optimizedSSAginac.hh"

namespace Fluc {
namespace Models {

OptimizedSSAginac::OptimizedSSAginac(libsbml::Model *model, int size, int seed) :
    StochasticSimulator(model, size, seed),
    ConstantStoichiometryMixin((BaseModel &)(*this)),
    //evalInterpreter(stateIndex),
    sparseStoichiometry(numSpecies(),numReactions()), dependencyGraph(numReactions(),numReactions()),
    priorityList(numReactions())
{

    // and compile propensities for byte code evaluation
    //for(size_t i=0; i<this->numReactions(); i++)
    //    this->interpreter.compileExpressionAndStore(this->propensities[i],i);

    // fill sparse stoichiometry
    for(size_t j=0; j<this->numReactions(); ++j)
    {
      this->sparseStoichiometry.startVec(j);              // optional for a DynamicSparseMatrix
      for(size_t i=0; i<this->numSpecies(); i++)        // with increasing i
          if (this->stoichiometry(i,j)!=0) this->sparseStoichiometry.insertBack(i,j) = this->stoichiometry(i,j);
    }
    this->sparseStoichiometry.finalize();

    // initialize dependency graph and priority list

    int d;
    for (size_t j=0;j<this->numReactions();j++)
    {
        this->priorityList(j)=j;
        this->dependencyGraph.startVec(j);
        for (size_t i=0;i<this->numReactions();i++)
        {
            d=0;
            for(size_t k=0;k<this->numSpecies();k++)
                d = d || ( this->reactants_stoichiometry(k,i)!=0 && this->stoichiometry(k,j)!=0 ) ;
            if (d!=0) this->dependencyGraph.insertBack(i,j) = d;
        }
    }
    this->dependencyGraph.finalize();

}

int
OptimizedSSAginac::select_reaction(const Eigen::VectorXd &prop, double &sum_propensity)

{

    double sum = 0.;
    double r = this->uniform()*sum_propensity;

    sum = prop(this->priorityList(0));

    size_t i=0;
    while(sum < r) sum += prop(this->priorityList(++i));

    int reaction = this->priorityList(i);

    // move reaction up in priority list
    if(i>0)
    {
        std::swap(priorityList(i),priorityList(i-1));
    }

    // do the following to reduce roundoff error
    if(i==this->numReactions()-1) sum_propensity = sum;

    return reaction;

}

void
OptimizedSSAginac::run(double step)

{

      // initialization
      double sum_propensity, newval;	        // sum of propensities
      double t,tau;			// time between reactions
      int reaction;			// reaction number selected

      Eigen::VectorXd prop(this->numReactions());

      for(int sid=0;sid<this->ensembleSize;sid++)
      {
          t=0;

          this->evaluate(this->observationMatrix.row(sid), prop);

          // initialize sum propensities
          sum_propensity = prop.sum();

 //         for(size_t i=0;i<numReactions();i++) this->priorityList(i)=i;

          while(t < step)
          {

              // sample tau
              if(sum_propensity > 0)
              {
                      tau = -std::log(this->uniform()) / sum_propensity;
              }
              else
              {
                  break;
                  //throw InternalError("SSA failed: negtive propensity encountered");
              }

              // select reaction
              reaction = this->select_reaction(prop, sum_propensity);

              // update chemical species
              for (Eigen::SparseMatrix<double>::InnerIterator it(this->sparseStoichiometry,reaction); it; ++it)
              {
                  this->observationMatrix(sid,it.row())+=it.value();
              }

              //this->observationMatrix.row(sid)+=this->stoichiometry.col(reaction);

              // first update values for state:
              this->interpreter.setValues(this->observationMatrix.row(sid));

              // update only propensities which are changed by the reaction
              for (Eigen::SparseMatrix<int>::InnerIterator it(this->dependencyGraph,reaction); it; ++it)
               {
                   //k = dependencyGraph(reaction,i);
                   newval = this->interpreter.evaluate(this->propensities[it.row()]);
                   sum_propensity += (newval-prop(it.row()));
                   prop(it.row()) = newval;
              }

              //this->evaluate(observationMatrix.row(sid),prop);
              //sum_propensity=prop.sum();

              //better recalculate if it is too small.
              if(t*sum_propensity <0.1){
                  this->evaluate(this->observationMatrix.row(sid),prop);
                  sum_propensity = prop.sum();
              }
              // time
              t += tau;

          } //end time step loop

      } // end ensemble loop

}


}
}
