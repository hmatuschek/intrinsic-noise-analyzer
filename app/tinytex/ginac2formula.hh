#ifndef __INA_APP_TINYTEX_GINAC2FORMULA_HH__
#define __INA_APP_TINYTEX_GINAC2FORMULA_HH__

#include <ginac.h>
#include "ast/scope.hh"
#include <QList>
#include <QVariant>
#include "formula.hh"


/** Simple class to traverse a @c GiNaC::ex expression and assemble a nice looking
 * representation of the expression.
 *
 * The symbols of the expressions are resolved within the given context. If the symbol
 * can be resolved and the resolved variable has a name, this name is used as a
 * displayname in the formula. If @c tex_names is @c true in the constructor, the
 * name will be rendered using @c TinyTex, allowing to use a rudimentary TeX within
 * variable names, note that only super and sub scripts as well as greek letters and some
 * arrows are allowed. */
class Ginac2Formula :
    public GiNaC::visitor, public GiNaC::symbol::visitor, public GiNaC::numeric::visitor,
    public GiNaC::add::visitor, public GiNaC::mul::visitor, public GiNaC::power::visitor,
    public GiNaC::basic::visitor
{
  /** Stack of temporary formula elements. */
  QList<MathItem *> _stack;
  /** A weak reference to the variable scope used to resolved GiNaC symbols. */
  iNA::Ast::Scope &_scope;
  /** Specifies if names should be rendered as TeX if enclosed in "$". */
  bool _tex_names;
  /** The current operator precedence. */
  int _current_precedence;

public:
  /** Constructor, needs the variable scope for symbol resolution. */
  Ginac2Formula(iNA::Ast::Scope &scope, bool tex_names=true);

  virtual void visit(const GiNaC::symbol &node);
  virtual void visit(const GiNaC::numeric &node);
  virtual void visit(const GiNaC::add &node);
  virtual void visit(const GiNaC::mul &node);
  virtual void visit(const GiNaC::power &node);
  virtual void visit(const GiNaC::basic &node);

  /** Retunrs the assembled formula. */
  MathItem *getFormula();

public:
  /** Renders an expression as a MathItem instance.
   * @param expression Specifies the expression to render.
   * @param scope Specifies the variable scope of the expression, this scope is used to resolve
   *        symbols in the given expression.
   * @param tex_names If the name of a resolved symbol is enclosed in "$", it may be rendered as
   *        tex. If this option is true, this will happen. */
  static MathItem *toFormula(GiNaC::ex expression, iNA::Ast::Scope &scope, bool tex_names=true);

  /** Very helpful function to render a given expression into a formula. The formula is returned
   * inside a QVariant as a pixmap or an invald QVariant if there was an error during rendering. */
  static QVariant toPixmap(GiNaC::ex expression, iNA::Ast::Scope &scope, bool tex_names=true);
};

#endif // GINAC2FORMULA_HH
