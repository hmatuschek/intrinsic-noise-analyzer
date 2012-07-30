#ifndef __INA_PARSER_SBMLSH_EXPORTER_HH__
#define __INA_PARSER_SBMLSH_EXPORTER_HH__

#include <ostream>


namespace Fluc {

namespace Ast {
// Forward declaration:
class Model;
}

namespace Parser {
namespace Sbmlsh {

/**
 * Serializes the given @c Ast::Model as SBML-SH into the given stream.
 */
void exportModel(Ast::Model &model, std::ostream &stream);

/**
 * Exports the given @c Ast::Model as SBML-SH into the given file.
 *
 * If the file does not exist, it will be created otherwise it will be overridden.
 */
void exportModel(Ast::Model &model, const std::string &filename);

}
}
}

#endif // __INA_PARSER_SBMLSH_EXPORTER_HH__
