#include "gillespieCLE.hh"

namespace Fluc {
namespace Models {

GillespieCLE::GillespieCLE(libsbml::Model *model, int size, int seed) :
    StochasticSimulator(model, size, seed),
    ConservationAnalysisMixin((BaseModel &)(*this)),
    reducedState(size,this->numIndSpecies())
{

    // save initial conditions
    this->ICsPermutated = this->PermutationM*this->ics;
    // compute conserved cycles in permutated base and store in conserved_cycles
    this->conserved_cycles=this->conservation_matrix*this->ICsPermutated;

    // initialize reduced state

    for(int ids=0; ids<this->ensembleSize; ids++)
    {
        this->reducedState.row(ids) = this->ICsPermutated.head(this->numIndSpecies());

    }

}


Eigen::VectorXd
GillespieCLE::full_state(const Eigen::VectorXd &state)

{
   // full state vector permutated
   Eigen::VectorXd Species_all(this->numSpecies());

   // reconstruct dependent species
   Eigen::VectorXd Species_dep;
   Species_dep = this->conserved_cycles+this->link_zero_matrix*state.head(this->numIndSpecies());

   // fill full state
   Species_all << state.head(this->numIndSpecies()), Species_dep;

   // restore original order and return
   return (this->PermutationM.transpose())*Species_all;

}

void
GillespieCLE::run(double step)

{

      double tau=0.0001;			// time step
      double sqrttau = std::sqrt(tau);

      for(int sid=0;sid<this->ensembleSize;sid++)
      {
          // initialization
          double t=0.;

          while(t < step)
          {

              // update propensities
            Eigen::VectorXd prop(this->numReactions());
              this->evaluate(this->observationMatrix.row(sid), prop);

              Eigen::MatrixXd noise,temp;
              Eigen::VectorXd drift, gwn;

              gwn = this->normal(this->numReactions());
              temp = (prop.cwiseSqrt()).asDiagonal();
              //std::cerr << ( (prop.cwiseSqrt()).asDiagonal() ) << std::endl;

              noise = this->reduced_stoichiometry*temp*gwn;
              drift = tau*this->reduced_stoichiometry*prop+sqrttau*noise;

              this->reducedState.row(sid) += (drift);


              // construct full state vector
              this->observationMatrix.row(sid) = full_state( this->reducedState.row(sid) );

              // time
              t += tau;

          } //end time step loop

      } // end ensemble loop

}

void
GillespieCLE::update_state(int &sid)

{
    //Eigen::VectorXd x;
    //x=this->reducedState.row(sid);
    //this->full_state(x,this->state.row(sid));
}

Eigen::VectorXd
GillespieCLE::normal(int size)
{

    Eigen::VectorXd ran(size);

    for(int i=0;i<size;i++)
    {
        ran(i)=normal();
    }

    return ran;

}

double
GillespieCLE::normal()

{

     static double n2 = 0.0;
     static int n2_cached = 0;
     if (!n2_cached) {
         // chose x,y
         double x, y, r;
         do {
             x = 2.0*this->uniform() - 1.;
             y = 2.0*this->uniform() - 1.;
             r = x*x + y*y;
         } while (r == 0.0 || r > 1.0);

         double d = sqrt(-2.0*log(r)/r);
         double n1 = x*d;
         n2 = y*d;

         double result = n1;

         n2_cached = 1;
         return result;

     } else {
         n2_cached = 0;
         return n2;
     }

}

double
GillespieCLE::uniform()

{

    return static_cast<double>(std::rand())/static_cast<double>(RAND_MAX);

}

}
}
