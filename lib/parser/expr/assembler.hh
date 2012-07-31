#ifndef __FLUC_PARSER_EXPR_ASSEMBLER_HH__
#define __FLUC_PARSER_EXPR_ASSEMBLER_HH__

#include <utils/parser.hh>
#include "parser.hh"


namespace Fluc {
namespace Parser {
namespace Expr {

/**
 * Internal used class to assemble @c GiNaC::ex expressions from CST.
 */
class Assembler
{
public:
  static GiNaC::ex processExpression(Utils::ConcreteSyntaxTree &expr, Context &ctx, Utils::Lexer &lexer);

protected:
  static GiNaC::ex processProduct(Utils::ConcreteSyntaxTree &expr, Context &ctx, Utils::Lexer &lexer);
  static GiNaC::ex processPower(Utils::ConcreteSyntaxTree &expr, Context &ctx, Utils::Lexer &lexer);
  static GiNaC::ex processAtomic(Utils::ConcreteSyntaxTree &expr, Context &ctx, Utils::Lexer &lexer);
  static GiNaC::ex processFunctionCall(Utils::ConcreteSyntaxTree &expr, Context &ctx, Utils::Lexer &lexer);
  static void processFunctionCallArguments(
    Utils::ConcreteSyntaxTree &expr, Context &ctx, Utils::Lexer &lexer, std::vector<GiNaC::ex> &args);
  static GiNaC::ex processNumber(Utils::ConcreteSyntaxTree &expr, Utils::Lexer &lexer);

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
