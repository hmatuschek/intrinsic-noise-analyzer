#ifndef INA_CLI_STEADYSTATE_HH
#define INA_CLI_STEADYSTATE_HH

#include <utils/option_parser.hh>

/** Performs the steady-state analysis as configured by the options in option-parser. */
int performSteadyStateAnalysis(iNA::Utils::Opt::Parser &option_parser);

#endif // INA_CLI_STEADYSTATE_HH
