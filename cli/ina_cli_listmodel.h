#ifndef INA_CLI_LISTMODEL_H
#define INA_CLI_LISTMODEL_H

#include <utils/option_parser.hh>
#include <ast/model.hh>

int listSpecies(iNA::Utils::Opt::Parser &option_parser);
int listCompartments(iNA::Utils::Opt::Parser &option_parser);
int listParameters(iNA::Utils::Opt::Parser &option_parser);
int setInitialValue(iNA::Ast::Model *model, iNA::Utils::Opt::Parser &option_parser);


#endif // INA_CLI_LISTMODEL_H
