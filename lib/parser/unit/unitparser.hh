#ifndef __INA_APP_TINYTEX_UNITPARSER_HH__
#define __INA_APP_TINYTEX_UNITPARSER_HH__

#include "ast/unitdefinition.hh"
#include "parser/lexer.hh"
#include "parser/production.hh"

namespace Fluc {
namespace Parser {
namespace Unit {


/** This class assembles a unit from its textual representation.
 * @todo Maybe move that into lib/parser. */
class UnitParser
{
public:
  /** Parses and assembles a unit from the given text. */
  static Fluc::Ast::Unit parse(const std::string &unit);
  /** Parses and assembles a unit from the text in the given stream. */
  static Fluc::Ast::Unit parse(std::istream &unit);

  /** Serializes a unit into the given stream. */
  static void write(const Ast::Unit &unit, std::ostream &output);

protected:
  /** Hidden constructor. */
  UnitParser();

  Fluc::Ast::Unit processUnit(Fluc::Parser::ConcreteSyntaxTree &node, Fluc::Parser::Lexer &lexer);
  Fluc::Ast::Unit processBaseUnit(Fluc::Parser::ConcreteSyntaxTree &node, Fluc::Parser::Lexer &lexer);
  Fluc::Ast::Unit processScale(Fluc::Parser::ConcreteSyntaxTree &node, Fluc::Parser::Lexer &lexer);
  Fluc::Ast::Unit processPow(Fluc::Parser::ConcreteSyntaxTree &node, Fluc::Parser::Lexer &lexer);


private:
  /** Tiny helper function to convert a string into a value. */
  template <typename Scalar> static Scalar asValue(const std::string &text) {
    std::stringstream buffer(text); Scalar value; buffer >> value; return value;
  }


protected:
  /** Lexer for units. */
  class Lexer : public Fluc::Parser::Lexer {
  public:
    typedef enum {
      UNIT_TOKEN = Fluc::Parser::Token::FIRST_USER_DEFINED, ///< An identifier for base units.
      FLOAT_TOKEN,   ///< A signed floating point number including exponent
      INTEGER_TOKEN, ///< A Signed integer token.
      TIMES_TOKEN,   ///< "*"
      DIVIDE_TOKEN,  ///< "/"
      POW_TOKEN,     ///< "^" | "**"
      LPAR_TOKEN,    ///< "("
      RPAR_TOKEN,    ///< ")"
      WHITESPACE_TOKEN ///< Any whitespace
    } TokenId;

  public:
    /** Constructor. */
    Lexer(std::istream &input);

  };

  /** Simple token rule to lex floating point numbers w/o sign. */
  class FloatTokenRule : public Fluc::Parser::TokenRule
  {
  public:
    /** Constrcutor. */
    FloatTokenRule(unsigned id);
  };

  /** Simple token rule to lex signed integers. */
  class IntegerTokenRule : public Fluc::Parser::TokenRule
  {
  public:
    /** Constructor. */
    IntegerTokenRule(unsigned id);
  };

  /** Grammar := [Unit] END_OF_INPUT */
  class GrammarProduction : public Fluc::Parser::Production
  {
  public:
    /** Factory method. */
    static Fluc::Parser::Production *factory();

  protected:
    /** Hidden constructor, use factory method. */
    GrammarProduction();
    /** Singleton instance. */
    static GrammarProduction *instance;
  };

  /** Unit := BaseUnit [("*"|"/") Unit]; */
  class UnitProduction : public Fluc::Parser::Production
  {
  public:
    /** Factory method. */
    static Fluc::Parser::Production *factory();

  protected:
    /** Hidden constructor. */
    UnitProduction();
    /** Singleton instance. */
    static UnitProduction *instance;
  };

  /** BaseUnit := (Scale | Pow | "(" Unit ")") */
  class BaseUnitProduction : Fluc::Parser::AltProduction
  {
  public:
    /** Factory method. */
    static Fluc::Parser::Production *factory();

  protected:
    /** Hidden constructor, use factory method. */
    BaseUnitProduction();
    /** Singleton instance. */
    static BaseUnitProduction *instance;
  };

  /** PowProduction := UnitId [('**'|'^') INTEGER] */
  class PowProduction : public Fluc::Parser::Production {
  public:
    /** Factory method. */
    static Fluc::Parser::Production *factory();

  protected:
    /** Hidden constructor. */
    PowProduction();
    /** Singleton instance. */
    static PowProduction *instance;
  };
};


}
}
}



#endif
