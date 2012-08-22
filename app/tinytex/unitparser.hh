#ifndef __INA_APP_TINYTEX_UNITPARSER_HH__
#define __INA_APP_TINYTEX_UNITPARSER_HH__

#include "ast/unitdefinition.hh"
#include "parser/lexer.hh"
#include "parser/production.hh"


/** This class assembles a unit from its textual representation. */
class UnitParser
{
public:
  /** Parses and assembles a unit from the given text. */
  static Fluc::Ast::Unit parse(const std::string &unit);
  /** Parses and assembles a unit from the text in the given stream. */
  static Fluc::Ast::Unit parse(std::istream &unit);


protected:
  /** Hidden constructor. */
  UnitParser();

  /** Assembles a unit form the given concrete syntax tree. */
  Fluc::Ast::Unit processUnit(Fluc::Parser::ConcreteSyntaxTree &node);
  /** Assembles a scaled base unit (as a Ast::Unit) given the concrete syntax tree. */
  Fluc::Ast::Unit processBaseUnit(Fluc::Parser::ConcreteSyntaxTree &node);


private:
  /** Maps base unit identifier to numeric base unit IDs. */
  std::map<std::string, Fluc::Ast::ScaledBaseUnit> _unit_table;


protected:
  /** Lexer for units. */
  class Lexer : public Fluc::Parser::Lexer {
  public:
    typedef enum {
      UNIT_TOKEN = Fluc::Parser::Token::FIRST_USER_DEFINED, ///< An identifier for base units.
      FLOAT_TOKEN,   ///< A floating point number.
      INTEGER_TOKEN, ///< A Signed integer token.
      TIMES_TOKEN,   ///< "*"
      DIVIDE_TOKEN,  ///< "/"
      POW_TOKEN,     ///< "**" | "^"
      LPAR_TOKEN,    ///< "("
      RPAR_TOKEN     ///< ")"
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



#endif
