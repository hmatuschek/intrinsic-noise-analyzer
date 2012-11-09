#include "ina_cli_paramscan.hh"
#include "ina_cli_importmodel.hh"
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


int saveParameterScan(std::vector<Eigen::VectorXd> &result, Utils::Opt::Parser &option_parser);

int performParamScan(Utils::Opt::Parser &option_parser)
{
  // Determine parameter identifier
  std::string identifier = option_parser.get_option("scan").front();

  // Determine parameter rage:
  std::string range = option_parser.get_option("range").front();
  double p_min, p_max; size_t N=100; bool success=true;
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
    success = success && string2Int(range.substr(idx_2+1), tmp); N = tmp;
    if (0 > tmp) { success = false; }
  } else {
    long int tmp;
    success = success && string2Int(range.substr(idx_1+1), tmp); N = tmp;
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
  double delta_p = (p_max-p_min)/(N-1);

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

  // Assemble parameter scan
  Models::IOSmodel ios_model(*model);
  std::vector< std::map<std::string, double> > parameter_sets(N);
  std::vector<Eigen::VectorXd> result(N);
  double value = p_min;
  for (size_t i=0; i<N; i++) {
    std::map<std::string, double> values; values[identifier] = value; value += delta_p;
    parameter_sets[i] = values;
    result[i] = Eigen::VectorXd(ios_model.getDimension());
  }

  // Show summary as debug message:
  Utils::Message message = LOG_MESSAGE(Utils::Message::DEBUG);
  message << "Perform parameter scan for parameter " << identifier
          << " from " << p_min << " to " << p_max << " in " << N << "steps.";
  Utils::Logger::get().log(message);

  // Run scan...
  Models::ParameterScan<Models::IOSmodel> pscan(ios_model);
  if (0 < pscan.parameterScan(parameter_sets, result)) {
    return -1;
  }

  return saveParameterScan(result, option_parser);
}


int saveParameterScan(std::vector<Eigen::VectorXd> &result, Utils::Opt::Parser &option_parser) {
  return -1;
}
