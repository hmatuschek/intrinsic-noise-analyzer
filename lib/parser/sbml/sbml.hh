#ifndef __INA_PARSER_SBML_SBML_HH__
#define __INA_PARSER_SBML_SBML_HH__

#include <ast/model.hh>


namespace Fluc {
namespace Parser {
namespace Sbml {


/** Imports an @c Ast::Model from the given SBML file.
 * @ingroup modelio */
void importModel(Ast::Model &model, const std::string &filename);

/** Creates and imports an @c Ast::Model from the given SBML file.
 * @ingroup modelio */
Ast::Model *importModel(const std::string &filename);

/** Exports an @c Ast::Model into the given file as SBML. */
void exportModel(Ast::Model &model, const std::string &filename);

}
}
}

#endif // __INA_PARSER_SBML_SBML_HH__
