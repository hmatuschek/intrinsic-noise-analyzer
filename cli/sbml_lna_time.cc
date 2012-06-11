#include "sbml_lna_time.hh"

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
  try
  {
    // Construct LNA model from SBML model
    Models::LinearNoiseApproximation model(doc->getModel());

    Models::SSEinterpreter<Models::LinearNoiseApproximation> interpreter(model, 0);

    double dt=0.01;
    ODE::RKF45<Models::SSEinterpreter<Models::LinearNoiseApproximation> > integrator(interpreter,dt,1e-5,1e-5);
    //ODE::RKF45<Models::LinearNoiseApproximation> integrator(model,dt,1e-5,1e-5);

    // state vector (deterministic concentrations + covariances)
    Eigen::VectorXd x(integrator.getDimension());
    Eigen::VectorXd dx(integrator.getDimension());

    // initialize state
    model.getInitialState(x);

    double t=0;
    for(int i=0; i<10000; i++){

       integrator.step(x,t,dx);
       x += dx; t += dt;

     }

  }
  catch (Exception err)
  {
    std::cerr << "Can not perform LNA: " << err.what() << std::endl;
    return -1;
  }
}
