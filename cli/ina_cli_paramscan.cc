#include "ina_cli_paramscan.hh"
#include "ina_cli_importmodel.hh"
#include <utils/logger.hh>
#include <models/steadystateanalysis.hh>
#include <models/IOSmodel.hh>


using namespace iNA;

int performParamScan(Utils::Opt::Parser &option_parser)
{
  // Determine parameter identifier
  std::string identifier = option_parser.get_option("scan").front();

  // Determine parameter rage:
  std::string range = option_parser.get_option("range").front();
  double p_min, p_max; size_t N=100;
  size_t idx_1 = range.find_first_of(':');
  if (idx_1 == std::string::npos) {
    Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
    message << "Invalid reange format in '" << range << "'. Must be FROM:TO[:STEPS]";
    Utils::Logger::get().log(message);
    return -1;
  }
  std::stringstream buffer; buffer.str(range.substr(0, idx_1)); buffer >> p_min;
  size_t idx_2 = range.find_first_of(':', idx_1+1);
  if (idx_2 != std::string::npos) {
    buffer.str(range.substr(idx_1+1, idx_2)); buffer >> p_max;
    buffer.str(range.substr(idx_2+1)); buffer >> N;
  } else {
    buffer.str(range.substr(idx_1+1)); buffer >> p_max;
  }
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

  Models::IOSmodel ios_model(*model);
  std::vector< std::map<std::string, double> > parameter_sets(N);
  std::vector<Eigen::VectorXd> result(N);
  double value = p_min;
  for (size_t i=0; i<N; i++) {
    std::map<std::string, double> values; values[identifier] = value; value += delta_p;
    parameter_sets[i] = values;
    result[i] = Eigen::VectorXd(ios_model.getDimension());
  }

  Models::ParameterScan<Models::IOSmodel> pscan(ios_model);
  pscan.parameterScan(parameter_sets, result);

  /// @bug Export scan.

  return -1;
}
