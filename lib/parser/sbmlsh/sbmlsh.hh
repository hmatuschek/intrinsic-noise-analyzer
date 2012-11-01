#ifndef __INA_PARSER_SBMLSH_SBMLSH_HH__
#define __INA_PARSER_SBMLSH_SBMLSH_HH__

#include <ast/model.hh>


namespace iNA {
namespace Parser {
namespace Sbmlsh {


/** Imports a SBML-sh model from the given stream.
 * @ingroup modelio */
void importModel(Ast::Model &model, std::istream &input);

/** Imports a SBML-sh model from the given file.
 * @ingroup modelio */
void importModel(Ast::Model &model, const std::string &filename);

/** Creates and imports a SBML-sh model from the given stream.
 * @ingroup modelio */
Ast::Model *importModel(std::istream &input);

/** Creates and imports a SBML-sh model from the given file.
 * @ingroup modelio  */
Ast::Model *importModel(const std::string &filename);


/** Serializes the given @c Ast::Model as SBML-sh into the given stream.
 * @ingroup modelio */
void exportModel(Ast::Model &model, std::ostream &stream);

/** Exports the given @c Ast::Model as SBML-sh into the given file.
 *
 * If the file does not exist, it will be created otherwise it will be overridden.
 * @ingroup modelio */
void exportModel(Ast::Model &model, const std::string &filename);


}
}
}
#endif
