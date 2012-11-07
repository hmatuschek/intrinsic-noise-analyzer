#include "ina_cli_importmodel.hh"
#include <parser/parser.hh>
#include <utils/logger.hh>


using namespace iNA;

std::string getFileExtension(const std::string &path) {
  size_t idx = path.find_last_of('.');
  if (idx == std::string::npos) { return ""; }
  return path.substr(idx+1);
}


Ast::Model *
importModel(Utils::Opt::Parser &option_parser)
{
  std::string filename = option_parser.get_option("model-sbml").front();

  if (option_parser.has_option("model-sbml")) {
    // Parse as SBML
    return Parser::Sbml::importModel(filename);
  } else if (option_parser.has_option("model-sbmlsh")) {
    // Parse as SBML-sh
    return Parser::Sbmlsh::importModel(filename);
  } else if (option_parser.has_option("model")){
    // Determine by file extension:
    std::string extension = getFileExtension(filename);
    if (("xml" == extension) || ("sbml" == extension)) {
      return Parser::Sbml::importModel(filename);
    } else if ("sbmlsh" == extension) {
      return Parser::Sbmlsh::importModel(filename);
    }
    // If extension is unknown:
    Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
    message << "Can not import model \"" << filename << "\" unrecognized file extension "
            << extension << std::endl;
    Utils::Logger::get().log(message);
    return 0;
  }

  // This sould not happen.
  std::cerr << "No model specified!" << std::endl;
  return 0;
}
