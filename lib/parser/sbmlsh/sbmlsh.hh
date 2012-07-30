#ifndef __INA_PARSER_SBMLSH_SBMLSH_HH__
#define __INA_PARSER_SBMLSH_SBMLSH_HH__

#include <ast/model.hh>


namespace Fluc {
namespace Parser {
namespace Sbmlsh {


/** Imports a SBML-SH model from the given stream. */
void importModel(Ast::Model &model, std::istream &input);

/** Imports a SBML-SH model from the given file. */
void importModel(Ast::Model &model, const std::string &filename);

/** Creates and imports a SBML-SH model from the given stream. */
Ast::Model *importModel(std::istream &input);

/** Creates and imports a SBML-SH model from the given file. */
Ast::Model *importModel(const std::string &filename);


/** Serializes the given @c Ast::Model as SBML-SH into the given stream. */
void exportModel(Ast::Model &model, std::ostream &stream);

/** Exports the given @c Ast::Model as SBML-SH into the given file.
 *
 * If the file does not exist, it will be created otherwise it will be overridden. */
void exportModel(Ast::Model &model, const std::string &filename);


}
}
}
#endif
