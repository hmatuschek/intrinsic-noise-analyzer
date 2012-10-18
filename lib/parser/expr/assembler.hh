#ifndef __FLUC_PARSER_EXPR_ASSEMBLER_HH__
#define __FLUC_PARSER_EXPR_ASSEMBLER_HH__

#include <parser/lexer.hh>
#include <parser/production.hh>
#include <ast/model.hh>
#include "parser.hh"


namespace iNA {
namespace Parser {
namespace Expr {


/**
 * Internal used class to assemble @c GiNaC::ex expressions from CST.
 *
 * This class is also used by the SBML-SH parser to parse expressions.
 */
class Assembler
{
protected:
  /** Holds the lexer. */
  Parser::Lexer &_lexer;
  /** Holds the parser context. */
  Context &_context;

public:
  /** Constructs the assembler for the given scope. */
  Assembler(Context &root, Parser::Lexer &lexer);

public:
  /** Entry point to parse an expression in the given context. */
  GiNaC::ex processExpression(Parser::ConcreteSyntaxTree &expr);
  /** Parses a product expression. */
  GiNaC::ex processProduct(Parser::ConcreteSyntaxTree &expr);
  /** Parses a power expression. */
  GiNaC::ex processPower(Parser::ConcreteSyntaxTree &expr);
  /** Parses an atomic expression. */
  GiNaC::ex processAtomic(Parser::ConcreteSyntaxTree &expr);
  /** Parses a function call. */
  GiNaC::ex processFunctionCall(Parser::ConcreteSyntaxTree &expr);
  /** Parses the arguments of a function call. */
  void processFunctionCallArguments(Parser::ConcreteSyntaxTree &expr, std::vector<GiNaC::ex> &args);
  /** Parses a number. */
  GiNaC::numeric processNumber(Parser::ConcreteSyntaxTree &expr);

public:
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
