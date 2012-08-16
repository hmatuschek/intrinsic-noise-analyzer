#ifndef __INA_APP_TINYTEX_TINYTEX_HH__
#define __INA_APP_TINYTEX_TINYTEX_HH__

#include "formula.hh"
#include "parser/lexer.hh"
#include "parser/production.hh"
#include "exception.hh"
#include <QVariant>


/** This class implements a trivial parser for a minimal subset of TeX to layout
 * formulas. This is used to handle @c Ast::VariableDefinition names as tex or to render
 * reaction equations.
 *
 * Although named TeX it is not compatible to TeX, it only handles expressions similar. It only
 * provides super and sup scripts and greek letters and some arrow symbols.
 */
class TinyTex
{
public:
  /** Exception class for all tinyTeX errors. */
  class Error : public Fluc::Exception {
  public:
    Error();
    Error(const Error &other);
    ~Error() throw ();
  };

public:
  /** Parses a simple formula and returns the equivalent MathFormulaItem. */
  static MathFormulaItem *parse(const std::string &source);

  /** Parses the given formula and returns the rendered pixmap, if rendering fails,
   * it returns the given source. */
  static QVariant toPixmap(const std::string &source);

protected:
  /** A weak reference to the lexer. */
  Fluc::Parser::Lexer &_lexer;
  /** A table to map symbols to its unicode string. */
  std::map<std::string, QString> _symbol_table;

protected:
  /** Hidden constructor, use @parse. */
  TinyTex(Fluc::Parser::Lexer &lexer);

  /** Assembles a MathFormula from the CST. */
  MathFormula *parseFormula(Fluc::Parser::ConcreteSyntaxTree &node);
  /** Assembles a super/sub script */
  MathFormulaItem *parseSupSub(Fluc::Parser::ConcreteSyntaxTree &node);
  /** Assembles a MathFormula element from the CST. */
  MathFormulaItem *parseElement(Fluc::Parser::ConcreteSyntaxTree &node);
  /** Unicode symbols. */
  MathFormulaItem *processSymbol(const std::string &symbol);


protected:
  /** Represents a simple word consisting of at least one char regexp: "[a-zA-Z]+". */
  class TextTokenRule : public Fluc::Parser::TokenRule {
  public:
    /** Constructor. */
    TextTokenRule(unsigned id);
  };

  /** Represents a single symbol token regexp: "'\'[a-zA-Z]+" */
  class SymbolTokenRule : public Fluc::Parser::TokenRule {
  public:
    /** Constructor. */
    SymbolTokenRule(unsigned id);
  };

  /** Implements the Lexer for TinyTex */
  class Lexer : public Fluc::Parser::Lexer {
  public:
    /** Enumerates all token identifiers. */
    typedef enum {
      TEXT_TOKEN = Fluc::Parser::Token::FIRST_USER_DEFINED, ///< A word.
      SYMBOL_TOKEN,       ///< A symbol "\SYMBOLNAME"
      SUP_TOKEN,          ///< '^'
      SUB_TOKEN,          ///< '_'
      LBRA_TOKEN,         ///< '{'
      RBRA_TOKEN,         ///< '}'
      WHITESPACE_TOKEN    ///< any whitespace
    } TokenId;

  public:
    /** Constructor. */
    Lexer(std::istream &input);
  };


  /** Grammar := Formula END_OF_FILE; */
  class GrammarProduction : public Fluc::Parser::Production {
  protected:
    /** Hidden constructor. */
    GrammarProduction();

  public:
    /** Factory method of the singleton. */
    static Fluc::Parser::Production *factory();

  private:
    /** Singleton instance. */
    static GrammarProduction *instance;
  };


  /** Formula := SupSubScript [Formula] */
  class FormulaProduction : public Fluc::Parser::Production {
  protected:
    /** Hidden constructor. */
    FormulaProduction();

  public:
    /** Factory method of the singleton. */
    static Fluc::Parser::Production *factory();

  private:
    /** Singleton instance. */
    static FormulaProduction *instance;
  };

  /** SupSubScript := Element [('^'|'_') Element] */
  class SupSubScriptProduction : public Fluc::Parser::Production
  {
  public:
    /** Factory method for the singleton. */
    static Fluc::Parser::Production *factory();

  protected:
    /** hidden constructor. */
    SupSubScriptProduction();
    static SupSubScriptProduction *instance;
  };

  /** Element := (TEXT | SYMBOL | '{' Formula '}') */
  class ElementProduction : public Fluc::Parser::AltProduction {
  protected:
    /** Hidden constructor. */
    ElementProduction();

  public:
    /** Factory method of the singleton. */
    static Fluc::Parser::Production *factory();

  private:
    /** Singleton instance. */
    static ElementProduction *instance;
  };
};

#endif // __INA_APP_TINYTEX_TINYTEX_HH__
