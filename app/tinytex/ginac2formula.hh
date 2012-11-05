#ifndef __INA_APP_TINYTEX_GINAC2customMULA_HH__
#define __INA_APP_TINYTEX_GINAC2customMULA_HH__

#include <ginac.h>
#include "ast/scope.hh"
#include <QList>
#include <QVariant>
#include "custommula.hh"
#include <parser/expr/ir.hh>


/** Simple expression context that resolves GiNaC symbols to the displayname of the associates
 * @c iNA::Ast::VariableDefinition. This class is used by the Ginac2custommula renderer to reverse
 * lookup GiNaC symbols in expressions. */
class ModelExpressionContext : public iNA::Parser::Expr::Context
{
public:
  /** Constructor. */
  ModelExpressionContext(const iNA::Ast::Scope &scope);
  /** Resolves a variable identifier to its symbol. */
  GiNaC::symbol resolve(const std::string &identifier);
  /** Resolves a GiNaC symbol to the name of the corresponding @c iNA::Ast::VariableDefinition.
   * If the variable has no name assigned, the identifier is returned. */
  std::string identifier(GiNaC::symbol symbol);
  /** Retruns true if the given symbol belongs to a species and if the model is defined in
   * substance units. Retruns always false if the scope is not element of an Model. */
  bool hasConcentrationUnits(GiNaC::symbol symbol);

private:
  /** Holds a weak reference to the scope context. */
  const iNA::Ast::Scope &_scope;
};



/** Simple class to translate a @c GiNaC::ex expression and assemble a nice looking
 * representation of the expression.
 *
 * The symbols of the expressions are resolved within the given context. If the symbol
 * can be resolved and the resolved variable has a name, this name is used as a
 * displayname in the custommula. If @c tex_names is @c true in the constructor, the
 * name will be rendered using @c TinyTex, allowing to use a rudimentary TeX within
 * variable names, note that only super and sub scripts as well as greek letters and some
 * arrows are allowed. */
class Ginac2custommula
{
  /** A weak reference to the variable scope used to resolved GiNaC symbols. */
  ModelExpressionContext &_context;
  /** Specifies if names should be rendered as TeX if enclosed in "$". */
  bool _tex_names;

public:
  /** Constructor, needs the variable scope custom symbol resolution. */
  Ginac2custommula(ModelExpressionContext &context, bool tex_names=true);

private:
  /** Translates the expression IR into MathItem representing the expression. */
  MathItem *_assemblecustommula(iNA::SmartPtr<iNA::Parser::Expr::Node> expression, size_t precedence);

public:
  /** Renders an expression as a MathItem instance.
   * @param expression Specifies the expression to render.
   * @param scope Specifies the variable scope of the expression, this scope is used to resolve
   *        symbols in the given expression.
   * @param tex_names If the name of a resolved symbol is enclosed in "$", it may be rendered as
   *        tex. If this option is true, this will happen. */
  static MathItem *tocustommula(GiNaC::ex expression, iNA::Ast::Scope &scope, bool tex_names=true);

  /** Very helpful function to render a given expression into a custommula. The custommula is returned
   * inside a QVariant as a pixmap or an invald QVariant if there was an error during rendering. */
  static QVariant toPixmap(GiNaC::ex expression, iNA::Ast::Scope &scope, bool tex_names=true);
};

#endif // GINAC2customMULA_HH
