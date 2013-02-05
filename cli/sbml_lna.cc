#include "sbml_lna.hh"
#include <parser/sbml/sbml.hh>
#include "unsupported.hh"

using namespace iNA;


int main(int argc, char *argv[])
{
  // Check args:
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " SBML_FILENAME" << std::endl;
    return -1;
  }

  // Construct LNA model from SBML model
  Ast::Model sbml_model; Parser::Sbmlsh::importModel(sbml_model, argv[1]);

  // Do the work:
  //try
  {
    Models::IOSmodel model(sbml_model);

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


    Eigen::VectorXd first(model.numSpecies());
    Eigen::MatrixXd second(model.numSpecies(),model.numSpecies());
    Eigen::VectorXd third(model.numSpecies());
    Eigen::VectorXd fourth(model.numSpecies());

    model.getCentralMoments(x,first,second,third,fourth);
    Eigen::VectorXd cm(4);
    cm(0) = first(0);
    cm(1) = second(0);
    cm(2) = third(0);
    cm(3) = fourth(0);
    Eigen::VectorXd ncm=iNA::Models::MaximumEntropyPDF::getNonCentralMoments(cm);

    std::cout << cm.transpose() << std::endl;
    std::cout << ncm.transpose() << std::endl;



    Eigen::MatrixXd fluxCOV;
    Eigen::MatrixXd fluxIOS;
    Eigen::VectorXd flux;
    Eigen::VectorXd fluxEMRE;


    std::cout << "SSE flux analysis:" << std::endl;
    std::cout << "==================" << std::endl;
    model.fluxAnalysis(x,flux,fluxEMRE,fluxCOV,fluxIOS);
    // output mean concentrations
    for(size_t i=0; i<model.numReactions(); i++)
    {
        std::cout<< model.getReaction(i)->getName() << ": \t" << flux(i) <<" (+/-"<< sqrt(fluxCOV(i,i)) <<")\t | "<< flux(i)+fluxEMRE(i)  <<" (+-"<< sqrt(fluxCOV(i,i)+fluxIOS(i,i)) << ")" << std::endl;
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

    std::cout << "SSA flux analysis:" << std::endl;
    std::cout << "==================" << std::endl;

    {
      // Construct SSA model from SBML model
      Models::OptimizedSSA model(sbml_model,30000,1024);
      double dt=0.1;

      Eigen::VectorXd mean;
      Eigen::MatrixXd variance;

      Eigen::MatrixXd cov(model.numSpecies(),model.numSpecies());

      for(double t=0.; t<50.; t+=dt)
      {
         model.run(dt);
      }

      model.fluxStatistics(mean,variance);

      // output mean concentrations
      for(size_t i=0; i<model.numReactions(); i++)
      {
          std::cout<< model.getReaction(i)->getName() << ": \t" << mean(i) <<" (+/-"<< sqrt(variance(i,i)) <<")\t" << std::endl;
      }

    }
  }

}
