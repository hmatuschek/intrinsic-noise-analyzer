#include <utils/option_parser.hh>
#include <utils/logger.hh>
#include <parser/parser.hh>
#include <iostream>

#include <models/IOSmodel.hh>
#include <models/steadystateanalysis.hh>

#include "ina_cli_steadystate.hh"


using namespace iNA;

Ast::Model *importModel(Utils::Opt::Parser &option_parser);


int main(int argc, const char *argv[])
{
  std::stringstream help_string;
  help_string
      << "iNA command line interface tool - version " << INA_VERSION_STRING << std::endl
      << std::endl
      << "Usage: " << std::endl
      << "  ina-cli [GLOBAL-OPTIONS] COMMAND MODEL [CMD-OPTIONS] OUTPUT" << std::endl
      << std::endl << std::endl
      << "GLOBAL-OPTIONS : Specifies some global options, independent of analysis command." << std::endl
      << "COMMAND        : The command to be executed, see below for details." << std::endl
      << "MODEL          : The model specification." << std::endl
      << "CMD-OPTIONS    : Command specific options." << std::endl
      << "OUTPUT         : Output specification." << std::endl
      << std::endl << std::endl
      << "Global Options:" << std::endl
      << " --loglevel=LEVEL" << std::endl
      << "               : Enables log messages and specifies the message level. LEVEL must be one" << std::endl
      << "                 of 'debug', 'info', 'warn' and 'error'." << std::endl
      << std::endl << std::endl
      << "Commands:" << std::endl
      << " --help        : Prints this help text." << std::endl
      << " --version     : Prints the version number of iNA." << std::endl
      << " --steadystate : Performs a steady state analysis." << std::endl
      << std::endl << std::endl
      << "Model specification:" << std::endl
      << " -m FILENAME, --model=FILENAME" << std::endl
      << "               : Specifies the model file, the file extension determines the model type." << std::endl
      << "                 *.xml and *.sbml are loaded as SBML and *.sbmlsh are loaded as SBML-sh." << std::endl
      << std::endl
      << " --model-sbml=FILENAME" << std::endl
      << "               : Specifies the SBML model file to load." << std::endl
      << std::endl
      << " --model-sbmlsh=FILENAME" << std::endl
      << "               : Specifies teh SBML-sh model file to load." << std::endl
      << std::endl << std::endl
      << "Ouput:" << std::endl
      << " -o FILENAME, --output=FILENAME" << std::endl
      << "               : Exports the analysis in the given file, the file type is determined by " << std::endl
      << "                 the file extension. *.csv = CSV, *.mat = Matlab version 5 file." << std::endl
      << std::endl
      << " --output-csv=FILENAME" << std::endl
      << "               : Exports the analysis data as CSV into the given filename" << std::endl
      << std::endl
      << " --output-stdcerr" << std::endl
      << "               : Exports the analysis data as CSV to stdout." << std::endl
      << std::endl
      << " --output-mat=FILENAME" << std::endl
      << "               : Exports the analysis data as MAT (Matlab version 5) file." << std::endl
      << std::endl
      << "Steady state analysis: --steadystate" << std::endl
      << "  If this command is present, iNA performs a steady state analysis ..." << std::endl
      << "  There are no command specific options for that analysis yet." << std::endl;

  // Global options:
  Utils::Opt::RuleInterface &loglevel_option = Utils::Opt::Parser::Option("loglevel");
  Utils::Opt::RuleInterface &global_options = Utils::Opt::Parser::opt(loglevel_option);

  // Version & help flags
  Utils::Opt::RuleInterface &version_flag = Utils::Opt::Parser::Flag("version");
  Utils::Opt::RuleInterface &help_flag = Utils::Opt::Parser::Flag("help");

  // Assemble model specifier
  Utils::Opt::RuleInterface &any_model = Utils::Opt::Parser::Option("model", 'm');
  Utils::Opt::RuleInterface &sbml_model = Utils::Opt::Parser::Option("model-sbml");
  Utils::Opt::RuleInterface &sbmlsh_model = Utils::Opt::Parser::Option("model-sbml");
  Utils::Opt::RuleInterface &model_specifier = (sbml_model | sbmlsh_model | any_model);

  // Assemble output specifier:
  Utils::Opt::RuleInterface &any_output = Utils::Opt::Parser::Option("output", 'o');
  Utils::Opt::RuleInterface &csv_output = Utils::Opt::Parser::Option("output-csv");
  Utils::Opt::RuleInterface &mat_output = Utils::Opt::Parser::Option("output-mat");
  Utils::Opt::RuleInterface &stdout_output = Utils::Opt::Parser::Flag("output-stdout");
  Utils::Opt::RuleInterface &output_specifier =
      (csv_output | mat_output | stdout_output | any_output);

  // Steady state analysis flag: --steadystate MODEL OUTPUT
  Utils::Opt::RuleInterface &steady_state_flag = Utils::Opt::Parser::Flag("steadystate");
  Utils::Opt::RuleInterface &steady_state_specifier =
      (steady_state_flag, model_specifier, output_specifier);

  // Task commands:
  Utils::Opt::RuleInterface &task_command = (global_options, steady_state_specifier);

  // Assemble option parser
  Utils::Opt::Parser option_parser((version_flag | help_flag | task_command));

  // If invalid argument -> print help
  if (! option_parser.parse(argv, argc)) {
    std::cout << help_string.str();
    return -1;
  }
  // If --help -> print help and exit:
  if (option_parser.has_flag("help")) {
    std::cout << help_string.str();
    return 0;
  }
  // Display version:
  if (option_parser.has_flag("version")) {
    std::cout << INA_VERSION_STRING << std::endl;
    return 0;
  }

  // Enable logging if log-level is set
  if (option_parser.has_option("loglevel")) {
    Utils::Message::Level level = Utils::Message::ERROR;
    std::string level_name = option_parser.get_option("loglevel").front();
    if ("debug" == level_name) { level = Utils::Message::DEBUG; }
    if ("info" == level_name) { level = Utils::Message::INFO; }
    if ("warn" == level_name) { level = Utils::Message::WARN; }
    if ("error" == level_name) { level = Utils::Message::ERROR; }
    Utils::Logger::get().addHandler(new Utils::TextMessageHandler(std::cerr, level));
  }

  // Dispatch by task:
  if (option_parser.has_flag("steadystate")) {
    return performSteadyStateAnalysis(option_parser);
  }

  std::cerr << "Unknown task selected!" << std::endl;
  return -1;
}






