#ifndef __FLUC_COMPILER_TRAFO_DUMP_HH__
#define __FLUC_COMPILER_TRAFO_DUMP_HH__

#include <ostream>

#include "ast/trafo/walker.hh"


namespace Fluc {
namespace Ast {
namespace Trafo {

/**
 * This class creates a string representation of an @c Ast::Module.
 *
 * @ingroup trafo
 */
class Dump : public Walker
{
protected:
  /**
   * Holds a weak reference to the output stream.
   */
  std::ostream &stream;

  /**
   * Current indentation.
   */
  size_t indent;


protected:
  /**
   * Performs the indentation.
   */
  void doIndent();


public:
  /**
   * Constructs a new serializer using thee given stream for string output.
   */
  Dump(std::ostream &stream);

  virtual void handleModule(Ast::Module *node);
  virtual void handleVariableDefinition(Ast::VariableDefinition *node);
  virtual void handleFunctionDefinition(Ast::FunctionDefinition *node);
  virtual void handleUnitDefinition(Ast::UnitDefinition *node);
  virtual void handleReaction(Ast::Reaction *node);
  virtual void handleKineticLaw(Ast::KineticLaw *node);

  virtual void handleExpression(GiNaC::ex node);


public:
  /**
   * Helper function to dump the string representation of an expression into the given string.
   *
   * @param node Specifies the expression to dump.
   * @param stream Specifies the stream to serialize the string representation into.
   */
  static void dump(GiNaC::ex node, std::ostream &stream);

};


}
}
}

#endif // DUMP_HH
