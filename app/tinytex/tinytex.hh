#ifndef __INA_APP_TINYTEX_TINYTEX_HH__
#define __INA_APP_TINYTEX_TINYTEX_HH__

#include "formula.hh"
#include "parser/lexer.hh"
#include "parser/production.hh"


/** This class implements a trivial parser for a minimal subset of TeX to layout
 * formulas. This is used to handle @c Ast::VariableDefinition names as tex. */
class TinyTex
{
protected:
  /** A weak reference to the lexer. */
  Fluc::Parser::Lexer &_lexer;
  /** A table to map symbols to its unicode string. */
  std::map<std::string, QString> _symbol_table;

protected:
  /** Hidden constructor, use @parse. */
  TinyTex(Fluc::Parser::Lexer &lexer);

  MathFormula *parseFormula(Fluc::Parser::ConcreteSyntaxTree &node);
  MathFormulaItem *parseElement(Fluc::Parser::ConcreteSyntaxTree &node);
  MathFormulaItem *processSymbol(const std::string &symbol);

public:
  /** Parses a simple formula and returns the equivalent MathFormulaItem. */
  static MathFormulaItem *parse(const std::string &source);

protected:
  /** Represents a simple word consisting of at least one char regexp: "[a-zA-Z]+". */
  class WordTokenRule : public Fluc::Parser::TokenRule {
  public:
    WordTokenRule(unsigned id);
  };

  /** Represents a single symbol token regexp: "'\'[a-zA-Z]+" */
  class SymbolTokenRule : public Fluc::Parser::TokenRule {
  public:
    SymbolTokenRule(unsigned id);
  };

  /** Implements the Lexer for TinyTex */
  class Lexer : public Fluc::Parser::Lexer {
  public:
    typedef enum {
      WORD_TOKEN = Fluc::Parser::Token::FIRST_USER_DEFINED,
      SYMBOL_TOKEN,
      NUMBER_TOKEN,
      SUP_TOKEN,
      SUB_TOKEN,
      LBRA_TOKEN,
      RBRA_TOKEN,
      WHITESPACE_TOKEN
    } TokenId;

  public:
    Lexer(std::istream &input);
  };


  /** Grammar := Formula END_OF_FILE; */
  class GrammarProduction : public Fluc::Parser::Production {
  protected:
    GrammarProduction();

  public:
    static Fluc::Parser::Production *factory();

  private:
    static GrammarProduction *instance;
  };


  /** Formula := Element [Formula] */
  class FormulaProduction : public Fluc::Parser::Production {
  protected:
    FormulaProduction();

  public:
    static Fluc::Parser::Production *factory();

  private:
    static FormulaProduction *instance;
  };


  /** Element := (WORD | SYMBOL | NUMBER | Element ('^'|'_') Element | '{' Formula '}') */
  class ElementProduction : public Fluc::Parser::AltProduction {
  protected:
    ElementProduction();

  public:
    static Fluc::Parser::Production *factory();

  private:
    static ElementProduction *instance;
  };
};

#endif // __INA_APP_TINYTEX_TINYTEX_HH__
