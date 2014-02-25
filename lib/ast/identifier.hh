#ifndef __INA_AST_IDENTIFIER_HH__
#define __INA_AST_IDENTIFIER_HH__

#include <string>
#include "../exception.hh"

namespace iNA {
namespace Ast {

/** Returns @c true if the given string is a valid identifier.
 * We use the same definition for identifiers as SBML does, hence a valid identifier has the
 * following form
 * \code
 *  ID := (letter | '_') (letter | number | '_')*
 * \endcode */
bool isValidId(const std::string &id);

}
}

/** Implements a simple assertion to check id a string is an identifier. */
#define INA_ASSERT_IDENTIFIER(id) if (! iNA::Ast::isValidId(id)) { \
  iNA::SymbolError err; \
  err << "In " << __FILE__ << " ( " << (unsigned int)(__LINE__) << "): Given string '" \
       << id << "' is not a valid identifier."; \
  throw err; \
}

#endif // __INA_AST_IDENTIFIER_HH__
