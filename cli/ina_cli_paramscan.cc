#include "ina_cli_paramscan.hh"
#include "ina_cli_importmodel.hh"
#include <utils/logger.hh>


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
  Ast::Parameter *param = model->getParameter(identifier);

  /// @bug Implement.

  return -1;
}
