#include "ina_cli_importmodel.hh"
#include "ina_cli_fileextension.hh"

#include <parser/parser.hh>
#include <utils/logger.hh>
#include "ina_cli_listmodel.h"


typedef enum {
  SBML_FILE, SBMLSH_FILE
} ModelType;


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
exportModel(Utils::Opt::Parser &option_parser)
{
  // First try to import a model:
  Ast::Model *model = importModel(option_parser);
  if (0 == model) { return -1; }

  // Determine filename and type:
  std::string filename; ModelType type;
  if (option_parser.has_option("export-sbml")) {
    filename = option_parser.get_option("export-sbml").front(); type=SBML_FILE;
  } else if (option_parser.has_option("export-sbmlsh")) {
    filename = option_parser.get_option("export-sbmlsh").front(); type=SBMLSH_FILE;
  } else if (option_parser.has_option("export")) {
    filename = option_parser.get_option("export").front();
    std::string extension = getFileExtension(filename);
    if (("xml" == extension) || ("sbml" == extension)) { type=SBML_FILE; }
    else if ("sbmlsh" == extension) { type = SBMLSH_FILE; }
    else {
      Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
      message << "Can not export model " << filename
              << ": Unkown file-extension: " << extension;
      Utils::Logger::get().log(message);
      return -1;
    }
  } else if (option_parser.has_flag("update")) {
    // Determine filename and type by input model:
    if (option_parser.has_option("model-sbml")) {
      filename = option_parser.get_option("model-sbml").front();
      type = SBML_FILE;
    } else if (option_parser.has_option("model-sbmlsh")) {
      filename = option_parser.get_option("model-sbmlsh").front();
      type = SBML_FILE;
    } else {
      filename = option_parser.get_option("model").front();
      std::string extension = getFileExtension(filename);
      if (("xml" == extension) || ("sbml" == extension)) { type=SBML_FILE; }
      else if ("sbmlsh" == extension) { type = SBMLSH_FILE; }
      else {
        Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
        message << "Can not export model " << filename
                << ": Unkown file-extension: " << extension;
        Utils::Logger::get().log(message);
        return -1;
      }
    }
  } else {
    // This sould not happen.
    Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
    message << "Internal Error: Unreachable reached!";
    Utils::Logger::get().log(message);
    return -1;
  }

  try {
    if (SBML_FILE == type) {
      Parser::Sbml::exportModel(*model, filename); return 0;
    } else {
      Parser::Sbmlsh::exportModel(*model, filename); return 0;
    }
  } catch (Exception &err) {
    // Handle error during import...
    Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
    message << "Can not import model " << filename << ": " << err.what();
    Utils::Logger::get().log(message);
    return -1;
  }

  // This sould not happen.
  Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
  message << "Internal Error: Unreachable reached!";
  Utils::Logger::get().log(message);

  return -1;
}
