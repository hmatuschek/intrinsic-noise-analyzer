#include "ina_cli_ssetimecourse.hh"
#include "ina_cli_utils.hh"
#include "ina_cli_savetable.hh"
#include "ina_cli_importmodel.hh"


#include <models/REmodel.hh>
#include <models/sseinterpreter.hh>
#include <ode/ode.hh>

using namespace iNA;
using namespace iNA::Utils;



int saveRETimecourseAnalysis(Eigen::MatrixXd &table, const Ast::Model &model, Opt::Parser &parser)
{
  // Assemble table header
  std::stringstream header;
  header << "# Result of RE time-course analysis:" << std::endl;
  // Assemble column names for table:
  std::vector<std::string> columns;
  columns.reserve(1+model.numSpecies());
  // First column -> time
  columns.push_back("time");
  // Remaining columns -> species concentrations
  for (size_t i=0; i<model.numSpecies(); i++) {
    columns.push_back(model.getSpecies(i)->getIdentifier());
    if (model.getSpecies(i)->hasName()) {
      columns[1+i] = model.getSpecies(i)->getName();
    }
  }
  // Finally save data
  return saveTable(header.str(), columns, "RE", table, parser);
}


int saveLNATimecourseAnalysis(Eigen::MatrixXd &table, const Ast::Model &model, Opt::Parser &parser)
{
  std::stringstream header; header << "# Result of LNA time-course analysis:" << std::endl;
  // Assemble column names for table:
  std::vector<std::string> columns;
  size_t N=model.numSpecies(); std::vector<std::string> species(N);
  columns.reserve(1+2*N+(N*(N+1))/2);
  // Assemble species names:
  for (size_t i=0; i<N; i++) {
    species[i] = model.getSpecies(i)->getIdentifier();
    if (model.getSpecies(i)->hasName()) { species[i] = model.getSpecies(i)->getName(); }
  }
  // First column -> time
  columns.push_back("time");
  // RE concentrations
  for (size_t i=0; i<N; i++) {
    std::stringstream buffer;
    buffer << "RE(" << species[i] << ")";
    columns.push_back(buffer.str());
  }
  // LNA covariance
  for (size_t i=0; i<N; i++) {
    for (size_t j=i; j<N; j++) {
      std::stringstream buffer;
      buffer << "COV("<<species[i]<<","<<species[j]<<")";
      columns.push_back(buffer.str());
    }
  }
  // EMRE concentrations
  for (size_t i=0; i<N; i++) {
    std::stringstream buffer;
    buffer << "EMRE(" << species[i] << ")";
    columns.push_back(buffer.str());
  }
  return saveTable(header.str(), columns, "LNA", table, parser);
}


int saveIOSTimecourseAnalysis(Eigen::MatrixXd &table, const Ast::Model &model, Opt::Parser &parser)
{
  std::stringstream header; header << "# Result of IOS time-course analysis:" << std::endl;
  // Assemble column names for table:
  std::vector<std::string> columns;
  size_t N=model.numSpecies(); std::vector<std::string> species(N);
  columns.reserve(1+3*N+(N*(N+1)));
  // Assemble species names:
  for (size_t i=0; i<N; i++) {
    species[i] = model.getSpecies(i)->getIdentifier();
    if (model.getSpecies(i)->hasName()) { species[i] = model.getSpecies(i)->getName(); }
  }
  // First column -> time
  columns.push_back("time");
  // RE concentrations
  for (size_t i=0; i<N; i++) {
    std::stringstream buffer;
    buffer << "RE(" << species[i] << ")";
    columns.push_back(buffer.str());
  }
  // LNA covariance
  for (size_t i=0; i<N; i++) {
    for (size_t j=i; j<N; j++) {
      std::stringstream buffer;
      buffer << "LNA_COV("<<species[i]<<","<<species[j]<<")";
      columns.push_back(buffer.str());
    }
  }
  // EMRE concentrations
  for (size_t i=0; i<N; i++) {
    std::stringstream buffer;
    buffer << "EMRE(" << species[i] << ")";
    columns.push_back(buffer.str());
  }
  // IOS covariance
  for (size_t i=0; i<N; i++) {
    for (size_t j=i; j<N; j++) {
      std::stringstream buffer;
      buffer << "IOS_COV("<<species[i]<<","<<species[j]<<")";
      columns.push_back(buffer.str());
    }
  }
  // IOS+EMRE concentrations
  for (size_t i=0; i<N; i++) {
    std::stringstream buffer;
    buffer << "IOS(" << species[i] << ")";
    columns.push_back(buffer.str());
  }

  return saveTable(header.str(), columns, "IOS", table, parser);
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

  return saveRETimecourseAnalysis(timeseries, *model, option_parser);
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
  size_t Ns = lna_model.numSpecies();
  // Holds the current system state (reduced state)
  Eigen::VectorXd x(lna_model.getDimension());
  // Holds the update to the next state (reduced state)
  Eigen::VectorXd dx(lna_model.getDimension());
  // Holds the concentrations for each species (full state)
  Eigen::VectorXd concentrations(Ns);
  // Holds the covariance matrix of species concentrations
  Eigen::MatrixXd cov(Ns, Ns);
  // Holds EMRE correction for state:
  Eigen::VectorXd emre(Ns);
  // Allocate output
  Eigen::MatrixXd timeseries(1+N_steps, 1+2*Ns+(Ns*(Ns+1))/2);

  // initialize (reduced) state
  lna_model.getInitialState(x);
  // get full initial concentrations and covariance
  lna_model.fullState(x, concentrations, cov, emre);
  /* Perform integration */
  double t  = t_min;

  // store initial state RE
  timeseries(0,0) = t;
  for (size_t j=0; j<Ns; j++) {
    timeseries(0, 1+j) = concentrations(j);
  }
  // store initial state LNA
  size_t idx = 1 + Ns;
  for (size_t j=0; j<Ns; j++) {
    for (size_t k=j; k<Ns; k++, idx++) {
      timeseries(0, idx) = cov(j, k);
    }
  }
  // store initial state EMRE
  for (size_t j=0; j<Ns; j++, idx++) {
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
    for (size_t j=0; j<Ns; j++) {
      timeseries(1+i, 1+j) = concentrations(j);
    }
    // Store cov() of species.
    size_t idx = 1 + Ns;
    for (size_t j=0; j<Ns; j++) {
      for (size_t k=j; k<Ns; k++, idx++) {
        timeseries(1+i, idx) = cov(j, k);
      }
    }
    // Store EMRE correction + LNA state:
    for (size_t j=0; j<Ns; j++, idx++) {
      timeseries(1+i, idx) = emre(j) + concentrations(j);
    }
  }

  return saveLNATimecourseAnalysis(timeseries, *model, option_parser);
}




int performIOSTimecourseAnalysis(iNA::Utils::Opt::Parser &option_parser)
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
  Models::IOSmodel ios_model(*model);

  // Using JIT compiler and LSODA integrator.
  Models::GenericSSEinterpreter<
      Models::IOSmodel,
      Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
      Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > interpreter(ios_model, 0);
  // Configure ODE integrator
  ODE::Stepper *stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
      Models::IOSmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
      Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
        interpreter, integration_range.getStepSize(), err_abs, err_rel);

  { // Short information about analysis:
    Utils::Message message = LOG_MESSAGE(Utils::Message::DEBUG);
    message << "Start IOS time-course analysis for model \"" << model->getIdentifier()
            << "\" from t=" << t_min << ".." << t_max << " in " << N_steps << " steps.";
    Utils::Logger::get().log(message);
  }

  // Holds the number of species:
  size_t Ns = ios_model.numSpecies();
  // Holds the current system state (reduced state)
  Eigen::VectorXd x(ios_model.getDimension());
  // Holds the concentrations for each species (full state)
  Eigen::VectorXd concentrations(Ns);
  // Holds the covariance matrix of species concentrations
  Eigen::MatrixXd lna(Ns, Ns);
  // Holds EMRE correction for state:
  Eigen::VectorXd emre(Ns);
  // Holds the covariance matrix of species concentrations
  Eigen::MatrixXd ios(Ns, Ns);
  // Holds the 3rd moment for each species.
  Eigen::VectorXd thirdMoment = Eigen::VectorXd::Zero(Ns);
  // Holds the iosemre for each species.
  Eigen::VectorXd iosemre(Ns);
  // Holds a row of the output-table:
  Eigen::MatrixXd timeseries(1+N_steps, 1+3*Ns+(Ns*(Ns+1)));

  // Assemble index tables:
  Eigen::VectorXi re_index_table(Ns), emre_index_table(Ns), ios_emre_index_table(Ns);
  Eigen::MatrixXi lna_index_table(Ns,Ns), ios_index_table(Ns,Ns);
  size_t column = 1;
  for (int i=0; i<(int)Ns; i++, column++) {
    re_index_table(i) = column;
  }
  for (int i=0; i<(int)Ns; i++) {
    for (int j=i; j<(int)Ns; j++, column++) {
      lna_index_table(i,j) = column; lna_index_table(j,i) = column;
    }
  }
  for (int i=0; i<(int)Ns; i++, column++) {
    emre_index_table(i) = column;
  }
  for (int i=0; i<(int)Ns; i++) {
    for (int j=i; j<(int)Ns; j++, column++) {
      ios_index_table(i,j) = column; lna_index_table(j,i) = column;
    }
  }
  for (int i=0; i<(int)Ns; i++, column++) {
    ios_emre_index_table(i) = column;
  }

  // initialize (reduced) state
  ios_model.getInitialState(x);
  // get full initial concentrations and covariance
  ios_model.fullState(x, concentrations, lna, emre, ios, thirdMoment, iosemre);

  // store initial state:
  timeseries(0, 0) = t_min;
  for (size_t i=0; i<Ns; i++)
  {
    timeseries(0, re_index_table(i)) = concentrations(i);
    timeseries(0, emre_index_table(i)) = concentrations(i);
    timeseries(0, ios_emre_index_table(i)) = concentrations(i);
    for (size_t j=i; j<Ns; j++){
      timeseries(0, lna_index_table(i,j)) = 0.0;
      timeseries(0, ios_index_table(i,j)) = 0.0;
    }
  }

  /*
   * Perfrom integration.
   */
  double t=t_min;
  for (size_t s=0; s<N_steps; s++)
  {
    // Update state & time
    stepper->step(x, t); t += dt;

    // Get full state:
    ios_model.fullState(x, concentrations, lna, emre, ios, thirdMoment, iosemre);

    // store state and time:
    timeseries(s,0) = t;
    for (size_t i=0; i<Ns; i++) {
      // Store means
      timeseries(s,re_index_table(i)) = concentrations(i);
      timeseries(s,emre_index_table(i)) = emre(i) + concentrations(i);
      timeseries(s,ios_emre_index_table(i)) = emre(i) + concentrations(i) + iosemre(i);
      // Store covariances:
      for (size_t j=i; j<Ns; j++){
        timeseries(s,lna_index_table(i,j)) = lna(i, j);
        timeseries(s,ios_index_table(i,j)) = lna(i, j) + ios(i, j);
      }
    }
  }

  // Done:
  return saveIOSTimecourseAnalysis(timeseries, *model, option_parser);
}
