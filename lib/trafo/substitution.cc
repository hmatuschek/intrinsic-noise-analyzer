#include "substitution.hh"
#include "exception.hh"


using namespace Fluc;
using namespace Fluc::Trafo;


Substitution::Substitution()
  : _substitution_table()
{
  // pass...
}


Substitution::Substitution(const GiNaC::exmap &table)
  : _substitution_table(table)
{
  // pass...
}


void
Substitution::_check_substitution_table()
{
  for (GiNaC::exmap::iterator item=_substitution_table.begin(); item!=_substitution_table.end(); item++) {
    if (item->first.has(item->second)) {
      SemanticError err;
      err << "Can not normalize substitution: " << item->first << " -> " << item->second
          << ": replacement contains replaced expression!";
      throw err;
    }
  }
}


void
Substitution::_normalize_substitution_table()
{
  if (0 == _substitution_table.size()) return;

  bool can_substitute = true;
  while (can_substitute) {
    GiNaC::exmap::iterator item=_substitution_table.begin();
    item->second = item->second.subs(_substitution_table);
    can_substitute = _has_substitue(item->second); item++;

    for (; item!=_substitution_table.end(); item++) {
      item->second = item->second.subs(_substitution_table);
      can_substitute |= _has_substitue(item->second);
    }

    _check_substitution_table();
  }
}


bool
Substitution::_has_substitue(GiNaC::ex expr)
{
  for (GiNaC::exmap::iterator item=_substitution_table.begin(); item!=_substitution_table.end(); item++) {
    if (expr.has(item->first)) { return true; }
  }

  return false;
}


void
Substitution::add(GiNaC::ex lhs, GiNaC::ex rhs, bool normalize)
{
  // Add substitution
  _substitution_table[lhs] = rhs;

  // If normalization is requested:
  if (normalize) {
    _normalize_substitution_table();
  }
}


void
Substitution::normalize() {
  _normalize_substitution_table();
}


const GiNaC::exmap &
Substitution::getTable() const
{
  return _substitution_table;
}


void
Substitution::act(Ast::VariableDefinition *var)
{
  // First traverse the AST further
  var->traverse(*this);

  // then, if there is an inital value defined for the variable.
  if (var->hasValue()) {
    var->setValue(var->getValue().subs(_substitution_table));
  }
}


void
Substitution::act(Ast::Rule *rule)
{
  // Perform substitutions:
  rule->setRule(rule->getRule().subs(_substitution_table));
}


void
Substitution::act(Ast::AlgebraicConstraint *constraint)
{
  // Perform substitutions
  constraint->setConstraint(constraint->getConstraint().subs(_substitution_table));
}


void
Substitution::act(Ast::Reaction *reac)
{
  // Traverse into kinetic law
  reac->traverse(*this);

  // handle reactants:
  for (Ast::Reaction::iterator item=reac->reacBegin(); item!=reac->reacEnd(); item++) {
    item->second = item->second.subs(_substitution_table);
  }

  // handle products:
  for (Ast::Reaction::iterator item=reac->prodBegin(); item!=reac->prodEnd(); item++) {
    item->second = item->second.subs(_substitution_table);
  }
}


void
Substitution::act(Ast::KineticLaw *law)
{
  // Traverse into local paramters:
  law->traverse(*this);

  // perform substitution on the rate law:
  law->setRateLaw(law->getRateLaw().subs(_substitution_table));
}
