#ifndef __FLUC_PARSER_EXPR_ASSEMBLER_HH__
#define __FLUC_PARSER_EXPR_ASSEMBLER_HH__

#include <utils/parser.hh>
#include <ast/model.hh>
#include "parser.hh"


namespace Fluc {
namespace Parser {
namespace Expr {


/**
 * Internal used class to assemble @c GiNaC::ex expressions from CST.
 *
 * This class is also used by the SBML-SH parser to parse expressions.
 */
class Assembler : public Context
{
protected:
  /** Holds the lexer. */
  Utils::Lexer &_lexer;

public:
  /** Constructs the assembler for the given scope. */
  Assembler(Ast::Scope *root, Utils::Lexer &lexer);

public:
  /** Entry point to parse an expression in the given context. */
  GiNaC::ex processExpression(Utils::ConcreteSyntaxTree &expr);

protected:
  /** Parses a product expression. */
  GiNaC::ex processProduct(Utils::ConcreteSyntaxTree &expr);
  /** Parses a power expression. */
  GiNaC::ex processPower(Utils::ConcreteSyntaxTree &expr);
  /** Parses an atomic expression. */
  GiNaC::ex processAtomic(Utils::ConcreteSyntaxTree &expr);
  /** Parses a function call. */
  GiNaC::ex processFunctionCall(Utils::ConcreteSyntaxTree &expr);
  /** Parses the arguments of a function call. */
  void processFunctionCallArguments(Utils::ConcreteSyntaxTree &expr, std::vector<GiNaC::ex> &args);
  /** Parses a number. */
  double processNumber(Utils::ConcreteSyntaxTree &expr);

protected:
  /** Tiny helper function to parse numbers from strings. */
  template <typename T>
  static T toNumber(const std::string &string)
  {
    T value;

    std::stringstream buffer(string);
    if (! (buffer>>value) ) {
      SBMLParserError err;
      err << "Can not parse \"" << string << "\" as " << typeid(T).name();
      throw err;
    }

    return value;
  }
};

}
}
}

#endif // __FLUC_PARSER_EXPR_ASSEMBLER_HH__
