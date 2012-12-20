#include "ina_cli_paramscan.hh"
#include "ina_cli_importmodel.hh"
#include "ina_cli_savetable.hh"

#include <utils/logger.hh>
#include <models/steadystateanalysis.hh>
#include <models/IOSmodel.hh>


using namespace iNA;


bool string2Double(const std::string &string, double &value) {
  const char *ptr = string.c_str();  char *endPtr = (char *)ptr;
  value = strtod(ptr, &endPtr);
  if (0 == value && ptr==endPtr) {
    return false;
  }
  return true;
}

bool string2Int(const std::string &string, long int &value) {
  const char *ptr = string.c_str();  char *endPtr = (char *)ptr;
  value = strtol(ptr, &endPtr, 10);
  if (0 == value && ptr==endPtr) {
    return false;
  }
  return true;
}


int saveParameterScan(Eigen::MatrixXd &result, Utils::Opt::Parser &option_parser);

int performParamScan(Utils::Opt::Parser &option_parser)
{
  // Determine parameter identifier
  std::string identifier = option_parser.get_option("scan").front();

  // Determine parameter rage:
  std::string range = option_parser.get_option("range").front();
  double p_min, p_max; size_t N_steps=100; bool success=true;
  size_t idx_1 = range.find_first_of(':');
  if (idx_1 == std::string::npos) {
    Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
    message << "Invalid reange format in '" << range << "'. Must be FROM:TO[:STEPS]";
    Utils::Logger::get().log(message);
    return -1;
  }
  success = success && string2Double(range.substr(0, idx_1), p_min);
  size_t idx_2 = range.find_first_of(':', idx_1+1);
  if (idx_2 != std::string::npos) {
    success = success && string2Double(range.substr(idx_1+1, idx_2-1-idx_1), p_max);
    long int tmp;
    success = success && string2Int(range.substr(idx_2+1), tmp); N_steps = tmp;
    if (0 > tmp) { success = false; }
  } else {
    long int tmp;
    success = success && string2Int(range.substr(idx_1+1), tmp); N_steps = tmp;
    if (0 > tmp) { success = false; }
  }

  // Check if range was parsed successfully:
  if (! success) {
    Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
    message << "Invalid reange format in '" << range << "'. Must be FROM:TO[:STEPS]";
    Utils::Logger::get().log(message);
    return -1;
  }

  // Determine step size
  double delta_p = (p_max-p_min)/(N_steps-1);

  // Load model
  Ast::Model *model = importModel(option_parser);
  if (0 == model) { return -1; }

  //Check for parameter:
  if (! model->hasParameter(identifier)) {
    Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
    message << "Can not perform parameter scan, parameter " << identifier << " is unknown.";
    Utils::Logger::get().log(message);
    return -1;
  }

  /// @todo Process paramscan specific options.
  // Assemble parameter scan
  Models::IOSmodel ios_model(*model);
  std::vector< std::map<std::string, double> > parameter_sets(N_steps);
  std::vector<Eigen::VectorXd> result(N_steps);
  double value = p_min;
  for (size_t i=0; i<N_steps; i++) {
    std::map<std::string, double> values; values[identifier] = value; value += delta_p;
    parameter_sets[i] = values;
    result[i] = Eigen::VectorXd(ios_model.getDimension());
  }

  // Show summary as debug message:
  Utils::Message message = LOG_MESSAGE(Utils::Message::DEBUG);
  message << "Perform parameter scan for parameter " << identifier
          << " from " << p_min << " to " << p_max << " in " << N_steps << "steps.";
  Utils::Logger::get().log(message);

  // Run scan...
  Models::ParameterScan<Models::IOSmodel> pscan(ios_model);
  if (0 > pscan.parameterScan(parameter_sets, result)) {
    return -1;
  }

  // Assemble result table
  size_t N_spec = model->numSpecies();
  size_t M = 3*N_spec + (N_spec*(N_spec+1)) + 1;
  Eigen::VectorXd re_means(N_spec), emre_means(N_spec), ios_means(N_spec), third(N_spec);
  Eigen::MatrixXd lna_cov(N_spec,N_spec), ios_cov(N_spec,N_spec);
  Eigen::MatrixXd result_table(N_steps,M);

  for (size_t i=0; i<N_steps; i++) {
    ios_model.fullState(result[i], re_means, lna_cov, emre_means, ios_cov, third, ios_means);
    size_t col = 0;

    // Store parameter value:
    result_table(i, 0) = parameter_sets[i][identifier]; col++;
    // Store RE means:
    for (size_t j=0; j<N_spec; j++, col++) { result_table(i, col) = re_means(j); }
    // Store LNA cov:
    for (size_t j=0; j<N_spec; j++) {
      for (size_t k=j; k<N_spec; k++, col++) {
        result_table(i,col) = lna_cov(j,k);
      }
    }
    // Store EMRE means:
    for (size_t j=0; j<N_spec; j++, col++) { result_table(i, col) = emre_means(j); }
    // Store IOS cov:
    for (size_t j=0; j<N_spec; j++) {
      for (size_t k=j; k<N_spec; k++, col++) {
        result_table(i,col) = ios_cov(j,k);
      }
    }
    // Store IOS means:
    for (size_t j=0; j<N_spec; j++, col++) { result_table(i, col) = ios_means(j); }
  }

  return saveParameterScan(result_table, option_parser);
}


int saveParameterScan(Eigen::MatrixXd &result, Utils::Opt::Parser &option_parser)
{
  std::stringstream header; header << "#Result of parameter scan" << std::endl;
  return saveTable(header.str(), "paramscan", result, option_parser);
}
