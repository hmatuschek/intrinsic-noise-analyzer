#include "assignmentruleinliner.hh"

using namespace Fluc;
using namespace Fluc::Trafo;


/* ******************************************************************************************** *
 * Implementation of AssignmentRuleCollector.
 * ******************************************************************************************** */
AssignmentRuleCollector::AssignmentRuleCollector(Substitution &subs, unsigned flags)
  : _substitutions(subs), _flags(flags)
{
  // Pass...
}

void
AssignmentRuleCollector::visit(const Ast::VariableDefinition *var)
{
  if ( (! var->hasRule()) || (Ast::Node::isAssignmentRule(var->getRule())) ) {
    return;
  }

  // If var is a species ...
  if ( Ast::Node::isSpecies(var) && (_flags & FROM_SPECIES) ) {
    _substitutions.add(var->getSymbol(), var->getRule()->getRule());
  } else if ( Ast::Node::isParameter(var) && (_flags & FROM_PARAMTERS)) {
    _substitutions.add(var->getSymbol(), var->getRule()->getRule());
  }
}



/* ******************************************************************************************** *
 * Implementation of AssignmentRuleInliner.
 * ******************************************************************************************** */
AssignmentRuleInliner::AssignmentRuleInliner(Ast::Model &model, unsigned flags)
  : Substitution(), _model(model)
{
  // First, collect assignment rules
  AssignmentRuleCollector collector(*this, flags);
  model.accept(collector);
}


void
AssignmentRuleInliner::act(Ast::Reaction *reac)
{
  // First, process stoichiometry and kinetic law:
  Substitution::act(reac);

  std::list<Ast::Species *> _remove_species;
  std::list<Ast::Species *> _add_species;

  // Check if a modifier is substituted:
  for (Ast::Reaction::mod_iterator item=reac->modBegin(); item!=reac->modEnd(); item++) {
    if (_substitution_table.end() != _substitution_table.find((*item)->getSymbol())) {
      _remove_species.push_back(*item);
      _get_referred_species(_substitution_table[(*item)->getSymbol()], _add_species);
    }
  }

  // Remove species from set of modifiers:
  for (std::list<Ast::Species *>::iterator item = _remove_species.begin();
       item != _remove_species.end(); ite++) {
    reac->remModifier(*item);
  }
  // Add replacements for modifier:
  for (std::list<Ast::Species *>::iterator item = _add_species.begin();
       item != _add_species.end(); item++) {
    reac->addModifier(*item);
  }
}


void
AssignmentRuleInliner::_get_referred_species(GiNaC::ex expr, std::list<Ast::Species *> &species)
{
  for (size_t i=0; i<_model.numSpecies(); i++) {
    if (expr.has(_model.getSpecies(i)->getSymbol())) {
      species.push_back(_model.getSpecies(i));
    }
  }
}


void
AssignmentRuleInliner::apply(Ast::Model &model, unsigned flags)
{
  AssignmentRuleInliner inliner(model, flags);
  model.apply(inliner);
}
