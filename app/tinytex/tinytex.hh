#ifndef __INA_APP_TINYTEX_TINYTEX_HH__
#define __INA_APP_TINYTEX_TINYTEX_HH__

#include "formula.hh"
#include "parser/lexer.hh"
#include "parser/production.hh"
#include "exception.hh"
#include <QVariant>
#include <ast/variabledefinition.hh>



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
  class Error : public iNA::Parser::ParserError {
  public:
    Error();
    Error(const Error &other);
    Error(const iNA::Parser::ParserError &other);
    ~Error() throw ();
  };

public:
  /** Parses a simple formula and returns the equivalent MathFormulaItem. */
  static MathItem *parse(const std::string &source);

  /** Parses the given formula and returns the rendered pixmap, if rendering fails,
   * it returns the given source. */
  static QVariant toPixmap(const std::string &source);

  /** Returns true if the string starts and ends with an '$'. */
  static bool isTexQuoted(const std::string &source);

  /** Removes the first and last '$'. */
  static std::string texUnquote(const std::string &source);

  /** Handles quoted strings. */
  static MathItem *parseQuoted(const std::string &source);

  /** Parses the name of a @c Ast::VariableDefinition if set otherwise the
   * identifier. */
  static MathItem *parseVariable(const iNA::Ast::VariableDefinition *var);


protected:
  /** A weak reference to the lexer. */
  iNA::Parser::Lexer &_lexer;
  /** A table to map symbols to its unicode string. */
  std::map<std::string, QString> _symbol_table;

protected:
  /** Hidden constructor, use @parse. */
  TinyTex(iNA::Parser::Lexer &lexer);

  /** Assembles a MathFormula from the CST. */
  MathFormula *parseFormula(iNA::Parser::ConcreteSyntaxTree &node);
  /** Assembles a super/sub script */
  MathItem *parseSupSub(iNA::Parser::ConcreteSyntaxTree &node);
  /** Assembles a MathFormula element from the CST. */
  MathItem *parseElement(iNA::Parser::ConcreteSyntaxTree &node);
  /** Unicode symbols. */
  MathItem *processSymbol(const std::string &symbol);


protected:
  /** Represents a simple word consisting of at least one char regexp: "[a-zA-Z]+". */
  class TextTokenRule : public iNA::Parser::TokenRule {
  public:
    /** Constructor. */
    TextTokenRule(unsigned id);
  };

  /** Represents a single symbol token regexp: "'\'[a-zA-Z]+" */
  class SymbolTokenRule : public iNA::Parser::TokenRule {
  public:
    /** Constructor. */
    SymbolTokenRule(unsigned id);
  };

  /** Implements the Lexer for TinyTex */
  class Lexer : public iNA::Parser::Lexer {
  public:
    /** Enumerates all token identifiers. */
    typedef enum {
      TEXT_TOKEN = iNA::Parser::Token::FIRST_USER_DEFINED, ///< A word.
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
  class GrammarProduction : public iNA::Parser::Production {
  protected:
    /** Hidden constructor. */
    GrammarProduction();

  public:
    /** Factory method of the singleton. */
    static iNA::Parser::Production *factory();

  private:
    /** Singleton instance. */
    static GrammarProduction *instance;
  };


  /** Formula := SupSubScript [Formula] */
  class FormulaProduction : public iNA::Parser::Production {
  protected:
    /** Hidden constructor. */
    FormulaProduction();

  public:
    /** Factory method of the singleton. */
    static iNA::Parser::Production *factory();

  private:
    /** Singleton instance. */
    static FormulaProduction *instance;
  };

  /** SupSubScript := Element [('^'|'_') Element] */
  class SupSubScriptProduction : public iNA::Parser::Production
  {
  public:
    /** Factory method for the singleton. */
    static iNA::Parser::Production *factory();

  protected:
    /** hidden constructor. */
    SupSubScriptProduction();
    static SupSubScriptProduction *instance;
  };

  /** Element := (TEXT | SYMBOL | '{' Formula '}') */
  class ElementProduction : public iNA::Parser::AltProduction {
  protected:
    /** Hidden constructor. */
    ElementProduction();

  public:
    /** Factory method of the singleton. */
    static iNA::Parser::Production *factory();

  private:
    /** Singleton instance. */
    static ElementProduction *instance;
  };
};

#endif // __INA_APP_TINYTEX_TINYTEX_HH__
