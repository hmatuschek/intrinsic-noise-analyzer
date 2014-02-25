#include "exprverification.hh"

using namespace iNA;


class ExprVerificator:
    public GiNaC::visitor, public GiNaC::symbol::visitor
{
public:
  ExprVerificator(Ast::Scope *scope): _scope(scope) {
    // pass...
  }

  virtual void visit(const GiNaC::symbol &symbol) {
    // Check if symbol is defined
    if (! _scope->hasVariable(symbol)) { _undefined_symbols.insert(symbol); }
  }

  inline void reset() {
    _undefined_symbols.clear();
  }

  inline const std::set<GiNaC::symbol, GiNaC::ex_is_less> &undefinedSymbols() const {
    return _undefined_symbols;
  }

protected:
  Ast::Scope *_scope;
  std::set<GiNaC::symbol, GiNaC::ex_is_less> _undefined_symbols;
};


bool
Trafo::verifyExpr(const GiNaC::ex &expr, Ast::Scope *scope) {
  ExprVerificator vis(scope);
  expr.traverse(vis);
  return 0 == vis.undefinedSymbols().size();
}
