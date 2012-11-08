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
      << std::endl
      << " --list-species, --list-compartments, --list-parameters" << std::endl
      << "               : Lists species, compartments or global paramters of the specified model." << std::endl
      << std::endl
      << " --set=ASSIGNMENT" << std::endl
      << "               : Sets the initial value of a species, compartment or global parameter. " << std::endl
      << "                 ASSIGNMEN has the form ID=EXPRESSION, where ID is the identifier of the " << std::endl
      << "                 species, compartment or global parameter to be set and expression is an" << std::endl
      << "                 arbitrary expression or value." << std::endl
      << std::endl
      << " --steadystate, --help-steadystate" << std::endl
      << "               : Performs a steady state analysis for the given model. --help-steadystate" << std::endl
      << "                 shows a details help about the analysis and the analysis specific options." << std::endl
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
      << "               : Specifies the SBML-sh model file to load." << std::endl
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
      << std::endl;

  // Assemble help for steady state analysis
  std::stringstream steadystate_help;
  steadystate_help
      << "iNA command line interface tool - version " << INA_VERSION_STRING << std::endl
      << std::endl
      << "Usage: " << std::endl
      << "  ina-cli [GLOBAL-OPTIONS] --steadystate MODEL [CMD-OPTIONS] OUTPUT" << std::endl
      << std::endl << std::endl
      << "About state analysis:" << std::endl
      << "  If this command is present, iNA performs a steady state analysis ..." << std::endl
      << std::endl << std::endl
      << "The steady state command specific options are:" << std::endl
      << "  --max-iter=N : Specifies the maximum number (N, integer) of iterations for convergence." << std::endl
      << "                 By default  = 100." << std::endl
      << "  --eps=F      : Specifies the absolute error (F, floating point) for the steady state." << std::endl
      << "                 By default = 1e-9." << std::endl
      << "  --max-df=F   : Specifies the maximum time step of intermediate integration to reach" << std::endl
      << "                 quadratic convergence. By default = 1e9." << std::endl
      << "  --min-df=F   : Specified the smalles time-step (F, floating point) for intermediate" << std::endl
      << "                 integration. By default = 1e-1."<< std::endl;

  // Global options:
  Utils::Opt::RuleInterface &loglevel_option = Utils::Opt::Parser::Option("loglevel");
  Utils::Opt::RuleInterface &global_options = Utils::Opt::Parser::opt(loglevel_option);

  // Version & help flags
  Utils::Opt::RuleInterface &version_flag = Utils::Opt::Parser::Flag("version");
  Utils::Opt::RuleInterface &help_flag = Utils::Opt::Parser::Flag("help");
  Utils::Opt::RuleInterface &steadystate_help_flag = Utils::Opt::Parser::Flag("help-steadystate");

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
  // Assemble steady state specific options
  Utils::Opt::RuleInterface &max_iter_option = Utils::Opt::Parser::Option("max-iter");
  Utils::Opt::RuleInterface &epsilon_option = Utils::Opt::Parser::Option("eps");
  Utils::Opt::RuleInterface &min_dt_option = Utils::Opt::Parser::Option("min-dt");
  Utils::Opt::RuleInterface &max_dt_option = Utils::Opt::Parser::Option("max-dt");
  Utils::Opt::RuleInterface &steadystate_options =
      Utils::Opt::Parser::zeroOrMore(
        (max_iter_option | epsilon_option | min_dt_option, max_dt_option));
  Utils::Opt::RuleInterface &steady_state_specifier =
      (steady_state_flag, model_specifier, steadystate_options, output_specifier);

  // Task commands:
  Utils::Opt::RuleInterface &task_command = (global_options, steady_state_specifier);

  // Assemble option parser
  Utils::Opt::Parser option_parser((version_flag | (help_flag | steadystate_help_flag) | task_command));

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
  // If --help-steadystate -> print help for steadystate analysis and exit
  if (option_parser.has_flag("help-steadystate")) {
    std::cout << steadystate_help.str();
    return 0;
  }
  // Display version:
  if (option_parser.has_flag("version")) {
    std::cout << INA_VERSION_STRING << std::endl;
    return 0;
  }

  // Set log level if option is presen
  Utils::Message::Level level = Utils::Message::ERROR;
  if (option_parser.has_option("loglevel")) {
    std::string level_name = option_parser.get_option("loglevel").front();
    if ("debug" == level_name) { level = Utils::Message::DEBUG; }
    if ("info" == level_name) { level = Utils::Message::INFO; }
    if ("warn" == level_name) { level = Utils::Message::WARN; }
    if ("error" == level_name) { level = Utils::Message::ERROR; }
  }
  Utils::Logger::get().addHandler(new Utils::TextMessageHandler(std::cerr, level));

  // Dispatch by task:
  if (option_parser.has_flag("steadystate")) {
    return performSteadyStateAnalysis(option_parser);
  }

  std::cerr << "Unknown task selected!" << std::endl;
  return -1;
}






