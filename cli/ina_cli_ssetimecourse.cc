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
  std::stringstream header; header << "# Result of RE time-course analysis:" << std::endl;
  return saveTable(header.str(), "RE", table, parser);
}

int saveLNATimecourseAnalysis(Eigen::MatrixXd &table, Opt::Parser &parser)
{
  std::stringstream header; header << "# Result of LNA time-course analysis:" << std::endl;
  return saveTable(header.str(), "LNA", table, parser);
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
    // Determine update and update state & time:
    try {
      stepper->step(x, t_min, dx); x+=dx; t_min += dt;
    } catch (iNA::Exception &err) {
      Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
      message << "Error during RE time-course analyis: "
              << err.what();
      Utils::Logger::get().log(message);
      return -1;
    }

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




int performLNATimecourseAnalysis(iNA::Utils::Opt::Parser &option_parser)
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
  Models::LNAmodel lna_model(*model);

  // Using JIT compiler and LSODA integrator.
  Models::GenericSSEinterpreter<
      Models::LNAmodel,
      Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
      Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > interpreter(lna_model, 0);
  // Configure ODE integrator
  ODE::Stepper *stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
      Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
      Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
        interpreter, integration_range.getStepSize(), err_abs, err_rel);

  { // Short information about analysis:
    Utils::Message message = LOG_MESSAGE(Utils::Message::DEBUG);
    message << "Start LNA time-course analysis for model \"" << model->getIdentifier()
            << "\" from t=" << t_min << ".." << t_max << " in " << N_steps << " steps.";
    Utils::Logger::get().log(message);
  }

  // Holds the number of species:
  size_t _Ns = lna_model.numSpecies();
  // Holds the current system state (reduced state)
  Eigen::VectorXd x(lna_model.getDimension());
  // Holds the update to the next state (reduced state)
  Eigen::VectorXd dx(lna_model.getDimension());
  // Holds the concentrations for each species (full state)
  Eigen::VectorXd concentrations(_Ns);
  // Holds the covariance matrix of species concentrations
  Eigen::MatrixXd cov(_Ns, _Ns);
  // Holds EMRE correction for state:
  Eigen::VectorXd emre(_Ns);
  // Allocate output
  Eigen::MatrixXd timeseries(1+N_steps, 1+2*_Ns+(_Ns*(_Ns+1))/2);

  // initialize (reduced) state
  lna_model.getInitialState(x);
  // get full initial concentrations and covariance
  lna_model.fullState(x, concentrations, cov, emre);
  /* Perform integration */
  double t  = t_min;

  // store initial state RE
  timeseries(0,0) = t;
  for (size_t j=0; j<_Ns; j++) {
    timeseries(0, 1+j) = concentrations(j);
  }
  // store initial state LNA
  size_t idx = 1 + _Ns;
  for (size_t j=0; j<_Ns; j++) {
    for (size_t k=j; k<_Ns; k++, idx++) {
      timeseries(0, idx) = cov(j, k);
    }
  }
  // store initial state EMRE
  for (size_t j=0; j<_Ns; j++, idx++) {
    timeseries(0, idx) = emre(j) + concentrations(j);
  }

  // Integration loop:
  for (size_t i=0; i<N_steps; i++)
  {
    // Determine update:
    stepper->step(x, t, dx); x+=dx; t += dt;

    // Get full state:
    lna_model.fullState(x, concentrations, cov, emre);

    // Store new time:
    timeseries(i+1, 0) = t;
    // Store states of selected species:
    for (size_t j=0; j<_Ns; j++) {
      timeseries(1+i, 1+j) = concentrations(j);
    }
    // Store cov() of species.
    size_t idx = 1 + _Ns;
    for (size_t j=0; j<_Ns; j++) {
      for (size_t k=j; k<_Ns; k++, idx++) {
        timeseries(1+i, idx) = cov(j, k);
      }
    }
    // Store EMRE correction + LNA state:
    for (size_t j=0; j<_Ns; j++, idx++) {
      timeseries(1+i, idx) = emre(j) + concentrations(j);
    }
  }

  return saveLNATimecourseAnalysis(timeseries, option_parser);
}
