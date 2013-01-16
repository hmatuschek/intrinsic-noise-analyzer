#include <utils/option_parser.hh>
#include <utils/logger.hh>
#include <parser/parser.hh>
#include <iostream>

#include <models/IOSmodel.hh>
#include <models/steadystateanalysis.hh>

#include "ina_cli_steadystate.hh"
#include "ina_cli_paramscan.hh"
#include "ina_cli_ssetimecourse.hh"
#include "ina_cli_ssa.hh"
#include "ina_cli_listmodel.h"
#include "ina_cli_importmodel.hh"



using namespace iNA;

Ast::Model *importModel(Utils::Opt::Parser &option_parser);


int main(int argc, const char *argv[])
{
  std::stringstream help_string;
  help_string
      << "iNA command line interface tool - version " << INA_VERSION_STRING << std::endl
      << std::endl
      << "Usage: " << std::endl
      << "  ina-cli [GLOBAL-OPTIONS] COMMAND [CMD-OPTIONS] MODEL OUTPUT" << std::endl
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
      << "               : Lists species, compartments or global parameters of the specified model." << std::endl
      << std::endl
      << " --export=FILE, --export-sbml=FILE, --export-sbmlsh, --update, --help-export" << std::endl
      << "               : Exports the model again. This can be used to update a model or to" << std::endl
      << "                 translate models between formats." << std::endl
      << std::endl
      << " --steadystate, --help-steadystate" << std::endl
      << "               : Performs a steady state analysis for the given model. --help-steadystate" << std::endl
      << "                 shows a details help about the analysis and the analysis specific options." << std::endl
      << std::endl
      << " --scan=ID, --help-scan" << std::endl
      << "               : Performs a scan over a parameter specified by ID." << std::endl
      << std::endl
      << " --re, --lna, --emre" << std::endl
      << "               : Performs a time-course analysis using the system size expansion." << std::endl
      << std::endl
      << " --ssa         : Performs a time-course analysis using the stochastic simulation algorithm." << std::endl
      << std::endl << std::endl
      << "Model specification:" << std::endl
      << " -m FILENAME, --model=FILENAME" << std::endl
      << "               : Specifies the model file, the file extension determines the model type." << std::endl
      << "                 *.xml and *.sbml are loaded as SBML and *.sbmlsh is loaded as SBML-sh." << std::endl
      << std::endl
      << " --model-sbml=FILENAME" << std::endl
      << "               : Specifies the SBML model file to load." << std::endl
      << std::endl
      << " --model-sbmlsh=FILENAME" << std::endl
      << "               : Specifies the SBML-sh model file to load." << std::endl
      << std::endl
      << " --set-value=ASSIGNMENT" << std::endl
      << "               : This option may be to set the initial value of a variable before" << std::endl
      << "                 performing any analysis. This option may be present multiple times to " << std::endl
      << "                 set several values."<< std::endl
      << std::endl << std::endl
      << "Ouput:" << std::endl
      << " -o FILENAME, --output=FILENAME" << std::endl
      << "               : Exports the analysis in the given file, the file type is determined by " << std::endl
      << "                 the file extension. *.csv = CSV, *.mat = Matlab version 5 file." << std::endl
      << std::endl
      << " --output-csv=FILENAME" << std::endl
      << "               : Exports the analysis data as CSV into the given filename" << std::endl
      << std::endl
      << " --output-stdout" << std::endl
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
      << " --max-iter=N  : Specifies the maximum number (N, integer) of iterations for convergence." << std::endl
      << "                 By default  = 100." << std::endl
      << " --eps=F       : Specifies the absolute error (F, floating point) for the steady state." << std::endl
      << "                 By default = 1e-9." << std::endl
      << " --max-df=F    : Specifies the maximum time step of intermediate integration to reach" << std::endl
      << "                 quadratic convergence. By default = 1e9." << std::endl
      << " --min-df=F    : Specified the smalles time-step (F, floating point) for intermediate" << std::endl
      << "                 integration. By default = 1e-1."<< std::endl;

  // Assemble help for export commands:
  std::stringstream export_help;
  export_help
      << "iNA command line interface tool - version " << INA_VERSION_STRING << std::endl
      << std::endl
      << "Usage: " << std::endl
      << "  ina-cli [GLOBAL-OPTIONS] EXPORT_COMMAND MODEL" << std::endl
      << std::endl << std::endl
      << "Export commands:" << std::endl
      << " --export=FILENAME" << std::endl
      << "               : Exports the loaded model into the given file. The file extension" << std::endl
      << "                 determines the export model type. *.xml and *.sbml are SBML files and" << std::endl
      << "                 *.sbmlsh are SBML-sh files." << std::endl
      << std::endl
      << " --export-sbml=FILENAME" << std::endl
      << "               : Exports the loaded model as SBML into the given file." << std::endl
      << std::endl
      << " --export-sbmlsh=FILENAME" << std::endl
      << "               : Exports the loaded model as SBML-sh into the given file." << std::endl
      << std::endl
      << " --update" << std::endl
      << "               : Updates the loaded model file. NOTE: iNA does not keep all annotations" << std::endl
      << "                 of the original model, these annotations may be lost when using this" << std::endl
      << "                 command." << std::endl;

  // Param scan help
  std::stringstream scan_help;
  scan_help
      << "iNA command line interface tool - version " << INA_VERSION_STRING << std::endl
      << std::endl
      << "Usage: " << std::endl
      << "  ina-cli [GLOBAL-OPTIONS] --scan=ID --range=RANGE MODEL OUTPUT" << std::endl
      << std::endl << std::endl
      << "Parameter scan:" << std::endl
      << " --scan=ID" << std::endl
      << "               : Specifies the parameter for the parameter scan." << std::endl
      << std::endl
      << " --range=RANGE" << std::endl
      << "               : Specifies the value range for the parameter scan. The range format is"<<std::endl
      << "                 FROM:TO[:STEPS].  FROM specifies the start value and TO specifies the"<<std::endl
      << "                 final value for  the parameter scan. N is optional  and specifies the"<<std::endl
      << "                 number steps. If not prensent, N is by default 100." << std::endl;


  // Global options:
  Utils::Opt::RuleInterface &loglevel_option = Utils::Opt::Parser::Option("loglevel");
  Utils::Opt::RuleInterface &global_options = Utils::Opt::Parser::opt(loglevel_option);

  // Version & help flags
  Utils::Opt::RuleInterface &version_flag = Utils::Opt::Parser::Flag("version");
  Utils::Opt::RuleInterface &help_flag = Utils::Opt::Parser::Flag("help");
  Utils::Opt::RuleInterface &steadystate_help_flag = Utils::Opt::Parser::Flag("help-steadystate");
  Utils::Opt::RuleInterface &export_help_flag = Utils::Opt::Parser::Flag("help-export");
  Utils::Opt::RuleInterface &scan_help_flag = Utils::Opt::Parser::Flag("help-scan");

  // Assemble model specifier
  Utils::Opt::RuleInterface &any_model = Utils::Opt::Parser::Option("model", 'm');
  Utils::Opt::RuleInterface &sbml_model = Utils::Opt::Parser::Option("model-sbml");
  Utils::Opt::RuleInterface &sbmlsh_model = Utils::Opt::Parser::Option("model-sbml");
  Utils::Opt::RuleInterface &set_value_option = Utils::Opt::Parser::Option("set-value");
  Utils::Opt::RuleInterface &model_specifier =
      ((sbml_model | sbmlsh_model | any_model), Utils::Opt::Parser::zeroOrMore(set_value_option));

  // Assemble output specifier:
  Utils::Opt::RuleInterface &any_output = Utils::Opt::Parser::Option("output", 'o');
  Utils::Opt::RuleInterface &csv_output = Utils::Opt::Parser::Option("output-csv");
  Utils::Opt::RuleInterface &mat_output = Utils::Opt::Parser::Option("output-mat");
  Utils::Opt::RuleInterface &stdout_output = Utils::Opt::Parser::Flag("output-stdout");
  Utils::Opt::RuleInterface &output_specifier =
      (csv_output | mat_output | stdout_output | any_output);

  // Model access commands
  Utils::Opt::RuleInterface &list_species_flag = Utils::Opt::Parser::Flag("list-species");
  Utils::Opt::RuleInterface &list_params_flag = Utils::Opt::Parser::Flag("list-parameters");
  Utils::Opt::RuleInterface &list_comps_flag = Utils::Opt::Parser::Flag("list-compartments");
  Utils::Opt::RuleInterface &list_model_command =
      ((list_species_flag|list_params_flag|list_comps_flag|set_value_option), model_specifier);

  // Steady state analysis flag: --steadystate MODEL OUTPUT
  Utils::Opt::RuleInterface &steady_state_flag = Utils::Opt::Parser::Flag("steadystate");
  Utils::Opt::RuleInterface &max_iter_option = Utils::Opt::Parser::Option("max-iter");
  Utils::Opt::RuleInterface &epsilon_option = Utils::Opt::Parser::Option("eps");
  Utils::Opt::RuleInterface &min_dt_option = Utils::Opt::Parser::Option("min-dt");
  Utils::Opt::RuleInterface &max_dt_option = Utils::Opt::Parser::Option("max-dt");
  Utils::Opt::RuleInterface &steadystate_options =
      Utils::Opt::Parser::zeroOrMore(
        (max_iter_option | epsilon_option | min_dt_option, max_dt_option));
  Utils::Opt::RuleInterface &steadystate_command =
      (steady_state_flag, model_specifier, steadystate_options, output_specifier);

  // Export commands
  Utils::Opt::RuleInterface &export_option = Utils::Opt::Parser::Option("export");
  Utils::Opt::RuleInterface &export_sbml_option = Utils::Opt::Parser::Option("export-sbml");
  Utils::Opt::RuleInterface &export_sbmlsh_option = Utils::Opt::Parser::Option("export-sbmlsh");
  Utils::Opt::RuleInterface &update_option = Utils::Opt::Parser::Option("update");
  Utils::Opt::RuleInterface &export_commands =
      ((export_option, export_sbml_option, export_sbmlsh_option, update_option), model_specifier);

  // Param scan commands
  Utils::Opt::RuleInterface &paramscan_option = Utils::Opt::Parser::Option("scan");
  Utils::Opt::RuleInterface &range_option = Utils::Opt::Parser::Option("range", 'R');
  Utils::Opt::RuleInterface &paramscan_command =
      (paramscan_option, range_option, model_specifier, output_specifier);

  // SSE timecourse analysis options
  Utils::Opt::RuleInterface &re_timecourse_flag = Utils::Opt::Parser::Flag("re");
  Utils::Opt::RuleInterface &lna_timecourse_flag = Utils::Opt::Parser::Flag("lna");
  Utils::Opt::RuleInterface &emre_timecourse_flag = Utils::Opt::Parser::Flag("emre");
  Utils::Opt::RuleInterface &sse_timecourse_command =
      ( (re_timecourse_flag | lna_timecourse_flag | emre_timecourse_flag),
        range_option, model_specifier, output_specifier );

  // SSA timecourse analysis options
  Utils::Opt::RuleInterface &ssa_timecourse_flag = Utils::Opt::Parser::Flag("ssa");
  Utils::Opt::RuleInterface &ensemble_size_option = Utils::Opt::Parser::Option("ensemble", 'N');
  Utils::Opt::RuleInterface &ssa_timecourse_command =
      ( ssa_timecourse_flag, range_option, ensemble_size_option, model_specifier, output_specifier );

  // Task commands:
  Utils::Opt::RuleInterface &task_command =
      (global_options,
       (steadystate_command | paramscan_command | sse_timecourse_command | ssa_timecourse_command |
        list_model_command | export_commands));

  // Help flags
  Utils::Opt::RuleInterface &help_flags =
      (help_flag | steadystate_help_flag | export_help_flag | scan_help_flag);

  // Assemble option parser
  Utils::Opt::Parser option_parser((version_flag | help_flags | task_command));

  /*
   * Parse arguments: If invalid argument -> print help
   */
  if (! option_parser.parse(argv, argc)) {
    std::cerr << help_string.str();
    return -1;
  }

  /*
   * Dispatch help flags.
   */
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
  // If --help-export -> print help for model export
  if (option_parser.has_flag("help-export")) {
    std::cout << export_help.str();
    return 0;
  }
  // If --help-scan -> print help for parameter scan and exit:
  if (option_parser.has_flag("help-scan")) {
    std::cout << scan_help.str();
    return 0;
  }
  /// @todo Write help for SSE time course analysis.
  /// @todo Write help for SSA time course analysis.
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


  /*
   * Dispatch by commands
   */
  if (option_parser.has_flag("steadystate")) {
    return performSteadyStateAnalysis(option_parser);
  } else if (option_parser.has_option("scan")) {
    return performParamScan(option_parser);
  } else if (option_parser.has_flag("re")) {
    return performRETimecourseAnalysis(option_parser);
  } else if (option_parser.has_flag("lna")) {
    return performLNATimecourseAnalysis(option_parser);
  } else if (option_parser.has_flag("emre")) {
    return performIOSTimecourseAnalysis(option_parser);
  } else if (option_parser.has_flag("ssa")) {
    return performSSATimecourseAnalysis(option_parser);
  } else if (option_parser.has_flag("list-species")) {
    return listSpecies(option_parser);
  } else if (option_parser.has_flag("list-compartments")) {
    return listCompartments(option_parser);
  } else if (option_parser.has_flag("list-parameters")) {
    return listParameters(option_parser);
  } else if (option_parser.has_option("export") || option_parser.has_option("export-sbml") ||
             option_parser.has_option("export-sbmlsh") || option_parser.has_option("update"))
  {
    return exportModel(option_parser);
  }

  std::cerr << "Unknown task selected!" << std::endl;
  return -1;
}






