#ifndef INA_CLI_LISTMODEL_H
#define INA_CLI_LISTMODEL_H

#include <utils/option_parser.hh>

int listSpecies(iNA::Utils::Opt::Parser &option_parser);
int listCompartments(iNA::Utils::Opt::Parser &option_parser);
int listParameters(iNA::Utils::Opt::Parser &option_parser);
int setInitialValue(iNA::Utils::Opt::Parser &option_parser);


#endif // INA_CLI_LISTMODEL_H
