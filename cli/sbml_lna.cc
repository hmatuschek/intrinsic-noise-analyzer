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

        Models::BaseModel base(doc->getModel());

        Models::REmodel model1(base);
        Models::REinterpreter inter(model1,2,1);

        // Construct LNA model from SBML model
        Models::LinearNoiseApproximation model(base);
        Models::SteadyStateAnalysisOld steadyState(model);


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

        std::cout<<concentrations.transpose()<<std::endl;
        std::cout<<(emre).transpose()<<std::endl;
        std::cout<<(iosemre).transpose()<<std::endl;

        std::cout<<std::endl<<std::endl;

        Models::IOSmodel modelN(base);
        Models::SteadyStateAnalysis<Models::IOSmodel> test(modelN);

        Eigen::VectorXd xi(modelN.getDimension());

        modelN.getInitialState(xi);
        test.calcSteadyState(xi);
        modelN.fullState(xi,concentrations,cov,emre,ioscov,thirdmoment,iosemre);

        std::cout<<concentrations.transpose()<<std::endl;
        std::cout<<iosemre.transpose()<<std::endl;

//    double dt=0.01;
//    ODE::LsodaDriver<Models::LNAinterpreter> integrator(interpreter,dt,1e-9,1e-5);


    // get full initial concentrations and covariance and emre




    //double t=0;
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
