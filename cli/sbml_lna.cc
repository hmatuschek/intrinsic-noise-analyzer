#include "sbml_lna.hh"

using namespace Fluc;


int main(int argc, char *argv[])
{
  // Check args:
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " SBML_FILENAME" << std::endl;
    return -1;
  }

  // Open document:
  libsbml::SBMLDocument *doc = libsbml::readSBMLFromFile(argv[1]);

  // Check for errors:
  if (0 < doc->getNumErrors())
  {
    std::cerr << "Error while reading SBML: ";
    doc->printErrors(std::cerr);
    return -1;
  }

  // Do the work:
  //try
  {

      // state vector (deterministic concentrations + covariances)



    Models::BaseModel base(doc->getModel());

    std::cerr << "List of parameters:"<<std::endl;
    for(size_t i =0; i<base.numParameters(); i++)
        std::cerr<<i<<"\t"<<base.getParameter(i)->getSymbol()<<"\t"<<base.getParameter(i)->getValue()<<std::endl;
    std::cerr<<std::endl<<std::endl;

    for(double delta=0.025; delta<0.3; delta+=0.025)
    {
        base.getParameter(0)->setValue(GiNaC::ex((1.-delta)/2.));

        // Construct LNA model from SBML model
        Models::LinearNoiseApproximation model(base);
        Models::SteadyStateAnalysis steadyState(model);

        Eigen::VectorXd x(model.getDimension());

        Eigen::VectorXd concentrations(model.numSpecies());
        Eigen::MatrixXd cov(model.numSpecies(),model.numSpecies());
        Eigen::VectorXd emre(model.numSpecies());

        Eigen::VectorXd thirdmoment(model.numSpecies());
        Eigen::MatrixXd ioscov(model.numSpecies(),model.numSpecies());
        Eigen::VectorXd iosemre(model.numSpecies());

        // initialize state
        model.getInitialState(x);

        steadyState.calcIOS(x);

        model.fullState(x,concentrations,cov,emre,ioscov,thirdmoment,iosemre);

        double theta = (cov(0,0)+ioscov(0,0))/cov(0,0);

        std::cout<<delta <<"\t"<<theta<<std::endl;

    }

//    Models::LNAinterpreter interpreter(model, 2, 1);

//    double dt=0.01;
//    ODE::LsodaDriver<Models::LNAinterpreter> integrator(interpreter,dt,1e-9,1e-5);


    // get full initial concentrations and covariance and emre




    double t=0;
    for(int i=0; i<100; i++){


       //model.full_state(x,concentrations,cov,emre);

       //if(i%100==0)
       {
           //std::cout<< t <<"\t";

//           // output mean concentrations
 //          for(size_t i=0; i<model.numSpecies(); i++)
           {
             // std::cout<< concentrations(i) <<"\t";
           }


           //           // output EMRE concentrations
//                      for(size_t i=0; i<model.numSpecies(); i++)
                      {
                      //   std::cout<< emre(i) <<"\t";
                      }

           //           // output variances
           //           for(size_t i=0; i<model.numSpecies(); i++)
           //           {
           //              std::cout<< cov(i,i)<<"\t";
           //           }

           //           // output covariances

           //           for(size_t i=0; i<model.numSpecies();i++)
           //           {
           //              for(size_t j=0; j<i;j++)
           //              {
           //                   std::cout<< cov(i,j)<<"\t";
           //              }
           //           }

           //std::cout << std::endl;
       }/*

       integrator.step(x,t,dx);
       x += dx; t += dt;


       model.fullState(x,concentrations,cov,emre,ioscov,thirdmoment,iosemre);

       //std::cout<< t <<std::endl;

       // output mean concentrations
       for(size_t i=0; i<model.numSpecies(); i++)
       {
          //std::cout<<concentrations(i) <<"\t";
       }*/

     }


  }
  //catch (Exception err)
  {
  //  std::cerr << "Can not perform LNA: " << err.str() << std::endl;
  //  return -1;
  }
}
