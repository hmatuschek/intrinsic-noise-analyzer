#ifndef INA_CLI_IMPORTMODEL_HH
#define INA_CLI_IMPORTMODEL_HH

#include <ast/model.hh>
#include <utils/option_parser.hh>

/** Imports a model as specified by the command line options. */
iNA::Ast::Model *importModel(iNA::Utils::Opt::Parser &option_parser);

/** Imports a model as specified by the command line options. */
int exportModel(iNA::Utils::Opt::Parser &option_parser);

/** Resets the initial values of the model as specified by the command line options. */
int setInitialValue(iNA::Ast::Model *model, iNA::Utils::Opt::Parser &option_parser);


#endif // INA_CLI_IMPORTMODEL_HH
