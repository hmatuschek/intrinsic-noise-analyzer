#include "sbml_ode_integrate.hh"
#include "option_parser.hh"


using namespace Fluc;


int main(int argc, char *argv[])
{
  /*
   * Assemble command line arguments parser.
   */
  std::list<IOptionRule *> flags;
  flags.push_back(new OptionRule("dump-module", true));

  std::list<IOptionRule *> options;
  options.push_back(new OptionRuleOptional(new OptionRuleOneOf(flags)));
  options.push_back(new OptionRuleValue());
  options.push_back(new OptionRuleValue());
  options.push_back(new OptionRuleValue());
  options.push_back(new OptionRuleValue());
  OptionParser option_parser(new OptionRuleList(options));

  if (! option_parser.parse(argv, argc))
  {
    std::cerr << option_parser.format_help(argv[0]);
    return -1;
  }

  // Extract some numbers:
  std::list<std::string> values = option_parser.get_values();
  double t0    = std::atof(values.front().c_str()); values.pop_front();
  double te    = std::atof(values.front().c_str()); values.pop_front();
  size_t steps = std::atoi(values.front().c_str()); values.pop_front();
  double dt    = (te-t0)/steps;


  /*
   * Open document:
   */
  libsbml::SBMLDocument *doc = libsbml::readSBMLFromFile(values.front().c_str()); values.pop_front();

  // Check for errors:
  if (0 < doc->getNumErrors())
  {
    std::cerr << "Error while reading SBML: ";
    doc->printErrors(std::cerr);
    return -1;
  }

  /*
   * Assemble and integrate ODE from SBML model.
   */
  try
  {
    // Construct general ODE from SBML model
    ODE::BaseModel model(doc->getModel());

    // If only the AST module is dumped:
    if (option_parser.has_flag("dump-module"))
    {
      model.getModule().dump(std::cout);
      return 0;
    }

    // Allocate state- and diff-vector:
    Eigen::VectorXd state(model.getDimension());
    Eigen::VectorXd delta(model.getDimension());

    // Dump header:
    std::vector<std::string> species;
    model.getVariableNames(species);
    std::cout << "#";
    for (size_t i=0; i<species.size(); i++)
    {
      std::cout << "\"" << species[i] << "\" ";
    }
    std::cout << std::endl;

    // Get init-values:
    model.getInitialValues(state);

    // Dump initial values:
    for (size_t i=0; i<model.getDimension(); i++)
    {
      std::cout << state[i] << " ";
    }
    std::cout << std::endl;

    // Assemble EulerDriver
    //ODE::EulerDriver<ODE::BaseModel> driver(model, dt);
    // Assemble RungeKutta4 driver:
    ODE::RungeKutta4<ODE::BaseModel> driver(model, dt);

    // Dump and update state
    for (size_t i=0; i<steps; i++)
    {
      // Calc Update:
      driver.step(state, t0, delta);

      // Update state
      state += delta;

      // Dump state:
      for (size_t j=0; j<model.getDimension(); j++)
      {
        std::cout << state(j) << " ";
      }
      std::cout << std::endl;
    }
  }
  catch (Exception err)
  {
    std::cerr << "Can not integrate ODE: " << err.str() << std::endl;
    return -1;
  }
}
