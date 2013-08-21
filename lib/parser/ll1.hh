#ifndef __INA_PARSER_LL1_HH__
#define __INA_PARSER_LL1_HH__

#include <set>

namespace iNA {
namespace Parser {

/** This namespace collects all classes forming the LL(1) parsers of iNA. These parsers will
 * replace the currently used recusive descending parsers and allow for a proper error discovery.
 * @deprecated Remove. */
namespace LL1 {


/** Abstract base class of all productions. */
class AbstractProduction
{
protected:
  /** Hidden constructor. */
  AbstractProduction();

public:
  /** Checks the grammar for conflicts. */
  virtual bool inConflictWith(AbstractProduction *other) = 0;

  /** Returns true if the given token matches one of the leafing tokens of the production. */
  bool startsWith(unsigned int token) const;

protected:
  /** Holds the set of leading tokes of the production. */
  std::set<unsigned int> _leading_token;
};


}
}
}

#endif // __INA_PARSER_LL1_HH__
