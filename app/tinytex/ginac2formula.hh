#ifndef __INA_APP_TINYTEX_GINAC2FORMULA_HH__
#define __INA_APP_TINYTEX_GINAC2FORMULA_HH__

#include <ginac.h>
#include "ast/scope.hh"
#include <QList>
#include "formula.hh"


/** Simple class to traverse a @c GiNaC::ex expression and assemble a nice looking
 * representation of the expression. */
class Ginac2Formula :
    public GiNaC::visitor, public GiNaC::symbol::visitor, public GiNaC::numeric::visitor,
    public GiNaC::add::visitor, public GiNaC::mul::visitor, public GiNaC::power::visitor
{
  QList<MathFormulaItem *> _stack;
  Fluc::Ast::Scope &_scope;
  bool _tex_names;
  int _current_precedence;

public:
  Ginac2Formula(Fluc::Ast::Scope &scope, bool tex_names=true);

  virtual void visit(const GiNaC::symbol &node);
  virtual void visit(const GiNaC::numeric &node);
  virtual void visit(const GiNaC::add &node);
  virtual void visit(const GiNaC::mul &node);
  virtual void visit(const GiNaC::power &node);

  MathFormulaItem *getFormula();
};

#endif // GINAC2FORMULA_HH
