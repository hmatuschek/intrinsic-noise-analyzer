#include "ina_cli_ssetimecourse.hh"
#include "ina_cli_utils.hh"
#include "ina_cli_savetable.hh"
#include "ina_cli_importmodel.hh"


#include <models/REmodel.hh>
#include <models/sseinterpreter.hh>
#include <ode/ode.hh>

using namespace iNA;
using namespace iNA::Utils;



int saveRETimecourseAnalysis(Eigen::MatrixXd &table, Opt::Parser &parser)
{
  std::stringstream header; header << "# Result of time-course analysis:" << std::endl;
  return saveTable(header.str(), "RE", table, parser);
}


int performRETimecourseAnalysis(iNA::Utils::Opt::Parser &option_parser)
{
  // Determine time rage:
  std::string range = option_parser.get_option("range").front();
  double t_min, t_max; size_t N_steps=100;
  if (! ina_cli_parseRange(range, t_min, t_max, N_steps)) {
    Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
    message << "Invalid range format in '" << range << "'. Must be FROM:TO[:STEPS]";
    Utils::Logger::get().log(message);
    return -1;
  }

  // Assemble constants
  ODE::IntegrationRange integration_range(t_min, t_max, N_steps);
  double err_abs = 1e-4, err_rel=1e-6;
  double dt = integration_range.getStepSize();

  // Load model and construct analysis:
  Ast::Model *model = importModel(option_parser);
  if (0 == model) { return -1; }
  Models::REmodel re_model(*model);

  // Using JIT compiler and LSODA integrator.
  Models::GenericSSEinterpreter<
      Models::REmodel,
      Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
      Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > interpreter(re_model, 0);
  // Configure ODE integrator
  ODE::Stepper *stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
      Models::REmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
      Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
        interpreter, integration_range.getStepSize(), err_abs, err_rel);

  { // Short information about analysis:
    Utils::Message message = LOG_MESSAGE(Utils::Message::DEBUG);
    message << "Start RE time-course analysis for model \"" << model->getIdentifier()
            << "\" from t=" << t_min << ".." << t_max << " in " << N_steps << " steps.";
    Utils::Logger::get().log(message);
  }

  // Holds the current system state (reduced state)
  Eigen::VectorXd x(re_model.getDimension());
  // Holds the update to the next state (reduced state)
  Eigen::VectorXd dx(re_model.getDimension());
  // Holds the concentrations for each species (full state)
  Eigen::VectorXd concentrations(re_model.numSpecies());
  // Holds the complete time-series:
  Eigen::MatrixXd timeseries(1+N_steps, 1+re_model.numSpecies());
  // initialize (reduced) state
  re_model.getInitialState(x);
  // get full initial concentrations and covariance
  re_model.fullState(x, concentrations);

  // store initial state:
  timeseries(0,0) = t_min;
  for (size_t j=0; j<re_model.numSpecies(); j++) {
    timeseries(0,1+j) = concentrations(j);
  }

  // Integration loop:
  for (size_t i=0; i<N_steps; i++)
  {
    // Determine update:
    stepper->step(x, t_min, dx);
    // Update state & time
    x+=dx; t_min += dt;
    // Get full state:
    re_model.fullState(x, concentrations);

    // Store new time:
    timeseries(i+1, 0) = t_min;
    // Store states of selected species:
    for (size_t j=0; j<re_model.numSpecies(); j++) {
      timeseries(i+1,1+j) = concentrations(j);
    }
  }

  return saveRETimecourseAnalysis(timeseries, option_parser);
}
