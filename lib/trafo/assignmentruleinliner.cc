#include "assignmentruleinliner.hh"

using namespace iNA;
using namespace iNA::Trafo;


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
  if ( Ast::Node::isSpecies(var) && (_flags & Filter::SPECIES) ) {
    _substitutions.add(var->getSymbol(), var->getRule()->getRule(), false);
  } else if ( Ast::Node::isParameter(var) && (_flags & Filter::PARAMETERS)) {
    _substitutions.add(var->getSymbol(), var->getRule()->getRule(), false);
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
  // Then normalize collected substitutions
  this->normalize();
}


void
AssignmentRuleInliner::act(Ast::Reaction *reac) {
  Substitution::act(reac);
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
