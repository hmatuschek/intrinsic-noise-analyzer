#include "gillespieSSAginac.hh"

namespace Fluc {
namespace Models {

GillespieSSAginac::GillespieSSAginac(libsbml::Model *model, int size, int seed) :
    StochasticSimulator(model, size, seed),
    ConstantStoichiometryMixin((BaseModel &)(*this))
{

}

int
GillespieSSAginac::selectReaction(const Eigen::VectorXd &prop, const double &sum_propensity)

{

    int reaction = -1;
    double sp = 0.;
    double r = this->uniform()*sum_propensity;
    for(size_t i=0; i<this->numReactions(); i++){
            sp += prop(i);
            if(r < sp){
               reaction = i;
               break;
            }
    }
    return reaction;

}

void
GillespieSSAginac::run(double step)

{

      // initialization
      double sum_propensity;	        // sum of propensities
      double t,tau;			// time between reactions

      int reaction;			// reaction number selected

      Eigen::VectorXd prop(this->numReactions());

      for(int sid=0;sid<this->ensembleSize;sid++)
      {
          t=0;
          while(t < step)
          {

              // update propensities
              this->evaluate(this->observationMatrix.row(sid), prop);

              sum_propensity = prop.sum();

              // sample tau
              if(sum_propensity > 0)
              {
                      tau = -std::log(this->uniform()) / sum_propensity;
              }
              else
              {
                      break;
              }

              // select reaction
              reaction = this->selectReaction(prop, sum_propensity);

              // update chemical species
              updateState(sid,reaction);

              // time
              t += tau;

          } //end time step loop

      } // end ensemble loop

}

void
GillespieSSAginac::updateState(int &sid, int &reaction)

{
    this->observationMatrix.row(sid)+=this->stoichiometry.col(reaction);
}


}
}
