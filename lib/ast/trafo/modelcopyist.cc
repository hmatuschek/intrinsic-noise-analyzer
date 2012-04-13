#include "modelcopyist.hh"
#include "ast/functiondefinition.hh"
#include "ast/reaction.hh"
#include "exception.hh"


using namespace Fluc;
using namespace Fluc::Ast::Trafo;


void
ModelCopyist::copy(Ast::Model *src, Ast::Model *dest)
{
  GiNaC::exmap translation_table;

  ModelCopyist::copy(src, dest, translation_table);
}


void
ModelCopyist::copy(Ast::Model *src, Ast::Model *dest, GiNaC::exmap &translation_table)
{
  // Table of species copies:
  std::map<Species *, Species *> species_table;

  // Sorry, iteration is quiet in-efficient, however:

  // Copy function definitions:
  for (Ast::Model::iterator iter = src->begin(); iter != src->end(); iter++)
  {
    if (Ast::Node::isFunctionDefinition(*iter)) {
      dest->addDefinition(ModelCopyist::copyFunctionDefinition(
                            static_cast<Ast::FunctionDefinition *>(*iter), translation_table));
    }
  }

  // Copy default units:
  dest->setDefaultSubstanceUnit(src->getDefaultSubstanceUnit().asScaledBaseUnit());
  dest->setDefaultVolumeUnit(src->getDefaultVolumeUnit().asScaledBaseUnit());
  dest->setDefaultAreaUnit(src->getDefaultAreaUnit().asScaledBaseUnit());
  dest->setDefaultLengthUnit(src->getDefaultLengthUnit().asScaledBaseUnit());
  dest->setDefaultTimeUnit(src->getDefaultTimeUnit().asScaledBaseUnit());

  // Copy user defined "specialized" units:
  for (Ast::Model::iterator iter = src->begin(); iter != src->end(); iter++)
  {
    if (Ast::Node::isUnitDefinition(*iter))
    {
      dest->addDefinition(ModelCopyist::copyUnitDefinition(
                            static_cast<Ast::UnitDefinition *>(*iter)));
    }
  }

  // Copy all parameter definitions:
  for (Ast::Model::iterator iter = src->begin(); iter != src->end(); iter++)
  {
    if (Ast::Node::isParameter(*iter))
    {
      dest->addDefinition(ModelCopyist::copyParameterDefinition(
                            static_cast<Ast::Parameter *>(*iter), translation_table));
    }
  }

  // Copy all compartments:
  for (Ast::Model::iterator iter = src->begin(); iter != src->end(); iter++)
  {
    if (Ast::Node::isCompartment(*iter))
    {
      dest->addDefinition(ModelCopyist::copyCompartmentDefinition(
                            static_cast<Ast::Compartment *>(*iter), translation_table));
    }
  }

  // Copy all species definition:
  for (Ast::Model::iterator iter = src->begin(); iter != src->end(); iter++)
  {
    if (Ast::Node::isSpecies(*iter))
    {
      dest->addDefinition(ModelCopyist::copySpeciesDefinition(
                            static_cast<Ast::Species *>(*iter), translation_table,
                            species_table, dest));
    }
  }

  // Copy constraints:
  for (Ast::Model::constraintIterator iter = src->constraintBegin();
       iter != src->constraintEnd(); iter++)
  {
    dest->addConstraint(ModelCopyist::copyConstraint(*iter, translation_table));
  }

  // Copy reactions:
  for (Ast::Model::iterator iter = src->begin(); iter != src->end(); iter++)
  {
    if (Ast::Node::isReactionDefinition(*iter)) {
      dest->addDefinition(ModelCopyist::copyReaction(
                            static_cast<Ast::Reaction *>(*iter), translation_table,
                            species_table));
    }
  }
}


Ast::FunctionDefinition *
ModelCopyist::copyFunctionDefinition(Ast::FunctionDefinition *node, GiNaC::exmap &translation_table)
{
  std::vector<Ast::VariableDefinition *> arguments(node->getNumArgs());
  for (size_t i=0; i<node->getNumArgs(); i++)
  {
    arguments[i] = new Ast::FunctionArgument(node->getArgByIdx(i).get_name());
    translation_table[node->getArgByIdx(i)] = arguments[i]->getSymbol();
  }

  GiNaC::ex body = node->getBody().subs(translation_table);

  return new Ast::FunctionDefinition(node->getIdentifier(), arguments, body);
}


Ast::UnitDefinition *
ModelCopyist::copyUnitDefinition(Ast::UnitDefinition *node)
{
  return new Ast::UnitDefinition(node->getIdentifier(), node->getUnit());
}


Ast::Parameter *
ModelCopyist::copyParameterDefinition(Ast::Parameter *node, GiNaC::exmap &translation_table)
{
  Ast::Parameter *param = new Ast::Parameter(node->getIdentifier(),
                                             node->getValue().subs(translation_table),
                                             node->getUnit(), node->isConst());
  // If parameter has a name:
  if (node->hasName()) {
    param->setName(node->getName());
  }

  // If parameter as a rule:
  if (node->hasRule())
  {
    param->setRule(ModelCopyist::copyRule(node->getRule(), translation_table));
  }

  // Store parameter symbol in translation-table:
  translation_table[node->getSymbol()] = param->getSymbol();

  // Done.
  return param;
}


Ast::Compartment *
ModelCopyist::copyCompartmentDefinition(Ast::Compartment *node, GiNaC::exmap &translation_table)
{
  // Copy compartment:
  Ast::Compartment *compartment = new Ast::Compartment(node->getIdentifier(),
                                                       node->getValue().subs(translation_table),
                                                       node->getUnit(), node->getDimension(),
                                                       node->isConst());

  // If compartment has a name:
  if (node->hasName()) {
    compartment->setName(node->getName());
  }

  // copy optional rules:
  if (node->hasRule())
  {
    compartment->setRule(ModelCopyist::copyRule(node->getRule(), translation_table));
  }

  // Store compartment symbol in translation-table:
  translation_table[node->getSymbol()] = compartment->getSymbol();

  // Done.
  return compartment;
}


Ast::Species *
ModelCopyist::copySpeciesDefinition(Ast::Species *node, GiNaC::exmap &translation_table,
                                    std::map<Ast::Species *, Ast::Species *> &species_table,
                                    Model *destination)
{
  // Get compartment associated to the copied species:
  Ast::Compartment *comp = destination->getCompartment(node->getCompartment()->getIdentifier());

  // Copy species:
  Ast::Species *species = new Ast::Species(node->getIdentifier(),
                                           node->getValue().subs(translation_table),
                                           node->getUnit(), node->hasOnlySubstanceUnits(), comp, node->getName(), node->isConst());

  // copy optional rules:
  if (node->hasRule())
  {
    species->setRule(ModelCopyist::copyRule(node->getRule(), translation_table));
  }

  // Store species in translation_table
  translation_table[node->getSymbol()] = species->getSymbol();
  species_table[node] = species;

  // Done.
  return species;
}


Ast::Constraint *
ModelCopyist::copyConstraint(Ast::Constraint *node, GiNaC::exmap &translation_table)
{
  // Dispatch by type:
  switch (node->getNodeType())
  {
  case Ast::Node::ALGEBRAIC_CONSTRAINT:
    return ModelCopyist::copyAlgebraicConstraint(
          static_cast<Ast::AlgebraicConstraint *>(node), translation_table);

  default:
    break;
  }

  InternalError err;
  err << "Can not copy constraint, unkown constraint type: " << node->getNodeType();
  throw err;
}


Ast::AlgebraicConstraint *
ModelCopyist::copyAlgebraicConstraint(Ast::AlgebraicConstraint *node, GiNaC::exmap &translation_table)
{
  return new Ast::AlgebraicConstraint(node->getConstraint().subs(translation_table));
}


Ast::Reaction *
ModelCopyist::copyReaction(Ast::Reaction *node, GiNaC::exmap &translation_table,
                           std::map<Ast::Species *, Ast::Species *> &species_table)
{
  // Copy kinetic law:
  Ast::KineticLaw *kinetic_law = ModelCopyist::copyKineticLaw(
        node->getKineticLaw(), translation_table);

  // Copy reaction:
  Ast::Reaction *reaction = new Ast::Reaction(
        node->getIdentifier(), kinetic_law, node->isReversible());

  // If reaction has a name:
  if (node->hasName()) {
    reaction->setName(node->getName());
  }

  // Copy reactants stoichiometry:
  for (Ast::Reaction::iterator iter = node->reacBegin(); iter != node->reacEnd(); iter++)
  {
    // Check if there is a replacement for species in species_table:
    if (species_table.end() == species_table.find(iter->first))
    {
      SymbolError err;
      err << "Can not copy reaction " << node->getIdentifier()
          << " there is no copy of species: " << iter->first->getIdentifier();
      throw err;
    }

    reaction->setReactantStoichiometry(
          species_table[iter->first], iter->second.subs(translation_table));
  }

  // Copy products stoichiometry:
  for (Ast::Reaction::iterator iter = node->prodBegin(); iter != node->prodEnd(); iter++)
  {
    // Check if there is a replacement for species in species_table:
    if (species_table.end() == species_table.find(iter->first))
    {
      SymbolError err;
      err << "Can not copy reaction " << node->getIdentifier()
          << " there is no copy of species: " << iter->first->getIdentifier();
      throw err;
    }

    reaction->setProductStoichiometry(
          species_table[iter->first], iter->second.subs(translation_table));
  }

  // Copy reaction modifiers:
  for (Ast::Reaction::mod_iterator iter = node->modBegin(); iter != node->modEnd(); iter++) {
    // Check if there is a replacement for the modifier in species_table:
    if (species_table.end() == species_table.find(*iter)) {
      SymbolError err;
      err << "Can not copy reaction " << node->getIdentifier()
          << " there is no copy of species: " << (*iter)->getIdentifier();
    }
    reaction->addModifier(species_table[(*iter)]);
  }

  // Done
  return reaction;
}


Ast::KineticLaw *
ModelCopyist::copyKineticLaw(Ast::KineticLaw *node, GiNaC::exmap &translation_table)
{
  // Create "empty" kinetic law:
  Ast::KineticLaw *kinetic_law = new Ast::KineticLaw(GiNaC::ex());

  // First, copy all local parameters:
  for (Ast::KineticLaw::iterator iter = node->begin(); iter != node->end(); iter++)
  {
    if (! Ast::Node::isParameter(*iter))
    {
      InternalError err;
      err << "Can not copy kinetic law: Law has local non-parameter variable defined!";
      throw err;
    }

    kinetic_law->addDefinition(ModelCopyist::copyParameterDefinition(
                                 static_cast<Ast::Parameter *>(*iter), translation_table));
  }

  // Copy rate law with local and global variables substituted:
  kinetic_law->setRateLaw(node->getRateLaw().subs(translation_table));

  // Done.
  return kinetic_law;
}


Ast::Rule *
ModelCopyist::copyRule(Ast::Rule *node, GiNaC::exmap &translation_table)
{
  // Dispatch by type:
  switch (node->getNodeType())
  {
  case Ast::Node::ASSIGNMENT_RULE:
    return ModelCopyist::copyAssignmentRule(
          static_cast<Ast::AssignmentRule *>(node), translation_table);

  case Ast::Node::RATE_RULE:
    return ModelCopyist::copyRateRule(
          static_cast<Ast::RateRule *>(node), translation_table);

  default:
    break;
  }

  InternalError err;
  err << "Can not copy rule: Unknown rule type: " << node->getNodeType();
  throw err;
}


Ast::AssignmentRule *
ModelCopyist::copyAssignmentRule(Ast::AssignmentRule *node, GiNaC::exmap &translation_table)
{
  return new Ast::AssignmentRule(node->getRule().subs(translation_table));
}


Ast::RateRule *
ModelCopyist::copyRateRule(Ast::RateRule *node, GiNaC::exmap &translation_table)
{
  return new Ast::RateRule(node->getRule().subs(translation_table));
}
