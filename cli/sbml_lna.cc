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
    // Construct LNA model from SBML model
    Models::IOSmodel model(doc->getModel());

    Models::IOSinterpreter interpreter(model, 2, 1);
    Models::SteadyStateAnalysis<Models::IOSmodel> steadyState(model);

    double dt=0.01;
    //ODE::RKF45<Models::LNAinterpreter> integrator(interpreter,dt,1e-5,1e-5);
    //ODE::RKF45<Models::LinearNoiseApproximation> integrator(model,dt,1e-9,1e-5);
    ODE::Rosenbrock4TimeInd<Models::IOSinterpreter> integrator(interpreter,dt,1e-9,1e-5);

    // state vector (deterministic concentrations + covariances)
    Eigen::VectorXd x(interpreter.getDimension());
    Eigen::VectorXd dx(interpreter.getDimension());

    Eigen::VectorXd concentrations(model.numSpecies());
    Eigen::MatrixXd cov(model.numSpecies(),model.numSpecies());
    Eigen::VectorXd emre(model.numSpecies());

    // initialize state
    interpreter.getInitialState(x);
    // get full initial concentrations and covariance and emre

    //interpreter.full_state(x, concentrations, cov, emre);

 //    Just dump all the nice expressions and values:
    steadyState.calcSteadyState(x);
    //model.fullState(x,concentrations,cov,emre);

    Eigen::MatrixXd fluxCOV;
    Eigen::MatrixXd fluxIOS;
    Eigen::VectorXd flux;

    model.fluxAnalysis(x,flux,fluxCOV,fluxIOS);
    // output mean concentrations
    for(size_t i=0; i<model.numReactions(); i++)
    {
        std::cout<< model.getReaction(i)->getName() << ": \t" << flux(i) <<"\t"<<sqrt(fluxCOV(i,i)) <<"\t"<<sqrt(fluxIOS(i,i)) << std::endl;
    }

    std::cout << std::endl << std::endl;

    //steadyState.dump(std::cerr);

    //std::cerr<<model.getTimeUnit().getMultiplier();

//    std::cout<<"Initial concentrations:"<<std::endl;
//    std::cout<<concentrations.transpose()<<std::endl;

//   std::cout<<"Initial covariance:"<<std::endl;
//   std::cout<<cov<<std::endl;

//    // header for output
//    std::cout<< "# t" <<"\t";

//    // mean concentrations
//    for(size_t i=0; i<model.numSpecies(); i++)
//    {
//        std::cout<< doc->getModel()->getSpecies(i)->getName() <<"\t";
//    }

//    // emre concentrations
//    for(size_t i=0; i<model.numSpecies(); i++)
//    {
//        std::cout<< doc->getModel()->getSpecies(i)->getName() <<"\t";
//    }

//    // variances
//    for(size_t i=0; i<model.numSpecies(); i++)
//    {
//        std::cout<< "var"<< doc->getModel()->getSpecies(i)->getName() <<"\t";
//    }

//    // covariances

//    for(size_t i=0; i<model.numSpecies();i++)
//    {
//       for(size_t j=0; j<i;j++)
//       {
//           std::cout<< "cov"<< doc->getModel()->getSpecies(i)->getName()
//                    << "-"  << doc->getModel()->getSpecies(j)->getName()
//                    << "\t";
//       }
//    }

    //std::cout<< std::endl;

    double t=0;
    for(int i=0; i<10000; i++){


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
       }

       integrator.step(x,t,dx);
       x += dx; t += dt;

     }

    //model.fullState(x,concentrations,cov,emre);

    std::cout<< t <<"\t";

    // output mean concentrations
    for(size_t i=0; i<model.numSpecies(); i++)
    {
      // std::cout<< concentrations(i) <<"\t";
    }

  }
  //catch (Exception err)
  {
  //  std::cerr << "Can not perform LNA: " << err.str() << std::endl;
  //  return -1;
  }
}
