#include <utils/option_parser.hh>
#include <parser/parser.hh>
#include <iostream>

#include <models/steadystateanalysis.hh>


using namespace iNA;

int performSteadyStateAnalysis(Utils::Opt::Parser &option_parser);


int main(int argc, const char *argv[])
{
  // Version & help flags
  Utils::Opt::RuleInterface &version_flag = Utils::Opt::Parser::Flag("version");
  Utils::Opt::RuleInterface &help_flag = Utils::Opt::Parser::Flag("help");

  // Assemble model specifier
  Utils::Opt::RuleInterface &any_model = Utils::Opt::Parser::Option("model");
  Utils::Opt::RuleInterface &sbml_model = Utils::Opt::Parser::Option("model-sbml");
  Utils::Opt::RuleInterface &sbmlsh_model = Utils::Opt::Parser::Option("model-sbml");
  Utils::Opt::RuleInterface &model_specifier = (sbml_model | sbmlsh_model | any_model);

  // Assemble output specifier:
  Utils::Opt::RuleInterface &any_output = Utils::Opt::Parser::Option("output");
  Utils::Opt::RuleInterface &csv_output = Utils::Opt::Parser::Option("output-csv");
  Utils::Opt::RuleInterface &mat_output = Utils::Opt::Parser::Option("output-mat");
  Utils::Opt::RuleInterface &stdout_output = Utils::Opt::Parser::Flag("output-stdout");
  Utils::Opt::RuleInterface &output_specifier =
      (csv_output | mat_output | stdout_output | any_output);

  // Steady state analysis flag: --steadystate MODEL OUTPUT
  Utils::Opt::RuleInterface &steady_state_flag = Utils::Opt::Parser::Flag("steadystate");
  Utils::Opt::RuleInterface &steady_state_specifier =
      (steady_state_flag, model_specifier, output_specifier);

  Utils::Opt::Parser option_parser(
        (version_flag | help_flag | steady_state_specifier));

  // If invalid argument -> print help
  if (! option_parser.parse(argv, argc)) {
    std::cout << option_parser.format_help(argv[0]);
    return -1;
  }
  // If --help -> print help and exit:
  if (option_parser.has_flag("help")) {
    std::cout << option_parser.format_help(argv[0]);
    return 0;
  }
  // Display version:
  if (option_parser.has_flag("version")) {
    std::cout << "iNA version" << INA_VERSION_STRING << std::cerr;
    return 0;
  }

  // Dispatch by task:
  if (option_parser.has_flag("steadystate")) {
    return performSteadyStateAnalysis(option_parser);
  }

  std::cerr << "Unknown task selected!" << std::endl;
  return -1;
}



int
performSteadyStateAnalysis(Utils::Opt::Parser &option_parser)
{
  // Parse model
  iNA::Ast::Model *model = 0;
  if (option_parser.has_option("sbml")) {
    model = Parser::Sbml::importModel(option_parser.get_option("sbml"));
  } else if (option_parser.has_option("sbmlsh")) {
    model = Parser::Sbmlsh::importModel(option_parser.get_option("sbmlsh"));
  } else {
    /// @todo Implement generic file pass
  }

  /// @todo Add some more options

  Models::SteadyStateAnalysis<Models::IOSModel> steadystate(*model);
  Eigen::VectorXd reduced_steadystate(steadystate.getDimension());
  steadystate.calcSteadyState(reduced_steadystate);

  Eigen::VectorXd re_steadystate(model->numSpecies());
  Eigen::VectorXd emre_steadystate(model->numSpecies());
  Eigen::VectorXd ios_steadystate(model->numSpecies());
  Eigen::MatrixXd lna_steadystate_cov(model->numSpecies(), model->numSpecies());
  Eigen::MatrixXd ios_steadystate_cov(model->numSpecies(), model->numSpecies());

}
