#include "ina_cli_importmodel.hh"
#include "ina_cli_fileextension.hh"

#include <parser/parser.hh>
#include <utils/logger.hh>
#include "ina_cli_listmodel.h"


using namespace iNA;

Ast::Model *
importModel(Utils::Opt::Parser &option_parser)
{
  std::string filename;
  try {
    if (option_parser.has_option("model-sbml")) {
      // Parse as SBML
      filename = option_parser.get_option("model-sbml").front();
      Ast::Model *model = Parser::Sbml::importModel(filename);
      if (0 != setInitialValue(model, option_parser)) { delete model; return 0; }
      return model;
    } else if (option_parser.has_option("model-sbmlsh")) {
      // Parse as SBML-sh
      filename = option_parser.get_option("model-sbmlsh").front();
      Ast::Model *model = Parser::Sbmlsh::importModel(filename);
      if (0 != setInitialValue(model, option_parser)) { delete model; return 0; }
      return model;
    } else if (option_parser.has_option("model")){
      // Determine by file extension:
          filename = option_parser.get_option("model").front();
      std::string extension = getFileExtension(filename);
      if (("xml" == extension) || ("sbml" == extension)) {
        Ast::Model *model = Parser::Sbml::importModel(filename);
        if (0 != setInitialValue(model, option_parser)) { delete model; return 0; }
        return model;
      } else if ("sbmlsh" == extension) {
        Ast::Model *model = Parser::Sbmlsh::importModel(filename);
        if (0 != setInitialValue(model, option_parser)) { delete model; return 0; }
        return model;
      }
      // If extension is unknown:
      Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
      message << "Can not import model \"" << filename << "\" unrecognized file extension "
              << extension << std::endl;
      Utils::Logger::get().log(message);
      return 0;
    }
  } catch (Exception &err) {
    // Handle error during import...
    Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
    message << "Can not import model " << filename << ": " << err.what();
    Utils::Logger::get().log(message);
    return 0;
  }

  // This sould not happen.
  std::cerr << "No model specified!" << std::endl;
  return 0;
}



int
exportModel(Ast::Model *model, Utils::Opt::Parser &option_parser)
{
  std::string filename;
  try {
    if (option_parser.has_option("model-sbml")) {
      filename = option_parser.get_option("model-sbml").front();
      Parser::Sbml::exportModel(*model, filename);
      return 0;
    } else if (option_parser.has_option("model-sbmlsh")) {
      // Parse as SBML-sh
      filename = option_parser.get_option("model-sbmlsh").front();
      Parser::Sbmlsh::exportModel(*model, filename);
      return 0;
    } else if (option_parser.has_option("model")){
      // Determine by file extension:
      filename = option_parser.get_option("model").front();
      std::string extension = getFileExtension(filename);
      if (("xml" == extension) || ("sbml" == extension)) {
        Parser::Sbml::exportModel(*model, filename);
        return 0;
      } else if ("sbmlsh" == extension) {
        Parser::Sbmlsh::exportModel(*model, filename);
        return 0;
      }
      // If extension is unknown:
      Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
      message << "Can not export model \"" << filename << "\" unrecognized file extension "
              << extension << std::endl;
      Utils::Logger::get().log(message);
      return -1;
    }
  } catch (Exception &err) {
    // Handle error during import...
    Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
    message << "Can not import model " << filename << ": " << err.what();
    Utils::Logger::get().log(message);
    return -1;
  }

  // This sould not happen.
  std::cerr << "No model specified!" << std::endl;
  return -1;
}
