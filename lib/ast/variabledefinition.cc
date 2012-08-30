#include "variabledefinition.hh"

using namespace iNA::Ast;


VariableDefinition::VariableDefinition(Node::NodeType type, const std::string &id,
                                       const Unit &unit, bool is_const)
  : Definition(id, type), has_value(false), is_const(is_const), symbol(id),
    value(), rule(0), unit(unit)
{
  // Done
}


VariableDefinition::VariableDefinition(Node::NodeType type, const std::string &id, GiNaC::ex value,
                                       const Unit &unit, const std::string &name, bool is_const)
  : Definition(id, name, type), has_value(true), is_const(is_const), symbol(id),
    value(value), rule(0), unit(unit)
{
  // Done.
}


VariableDefinition::VariableDefinition(Node::NodeType type, const std::string &id, GiNaC::ex value, Rule *rule,
                                       const Unit &unit, bool is_const)
  : Definition(id, type), has_value(true), is_const(is_const), symbol(id),
    value(value), rule(rule), unit(unit)
{
  // Done.
}


VariableDefinition::~VariableDefinition()
{
  // Free attached rule if there is one:
  if (0 != this->rule)
  {
      delete this->rule;
  }
}


bool VariableDefinition::isConst() const {
  return this->is_const;
}

void VariableDefinition::setConst(bool is_const) {
  this->is_const = is_const;
}

bool VariableDefinition::hasValue() const {
  return this->has_value;
}

GiNaC::ex VariableDefinition::getValue() const {
  return this->value;
}

void VariableDefinition::setValue(GiNaC::ex value) {
  this->value = value;
  this->has_value = true;
}

const GiNaC::symbol & VariableDefinition::getSymbol() const {
  return this->symbol;
}

bool VariableDefinition::hasRule() const {
  return 0 != this->rule;
}

Rule * VariableDefinition::getRule() {
  return this->rule;
}

const Rule * VariableDefinition::getRule() const {
  return this->rule;
}

void VariableDefinition::setRule(Rule *rule) {
  this->rule = rule;
}

bool VariableDefinition::isDimensionLess() const {
  return this->unit.isDimensionless();
}

const Unit & VariableDefinition::getUnit() const {
  return this->unit;
}

void VariableDefinition::setUnit(const Unit &unit) {
  this->unit = unit;
}


void
VariableDefinition::dump(std::ostream &str)
{
  if (this->is_const) {
    str << "const " << this->getIdentifier();
  } else {
    str << "var " << this->getIdentifier();
  }

  if (this->hasValue()) {
    str << " = " << this->value;
  }

  if (this->hasRule()) {
    this->rule->dump(str);
  }
}


void
VariableDefinition::accept(Ast::Visitor &visitor) const
{
  if (VariableDefinition::Visitor *var_vis = dynamic_cast<VariableDefinition::Visitor *>(&visitor)) {
    var_vis->visit(this);
  } else {
    VariableDefinition::traverse(visitor);
    Definition::accept(visitor);
  }
}


void
VariableDefinition::apply(Ast::Operator &op)
{
  if (VariableDefinition::Operator *var_op = dynamic_cast<VariableDefinition::Operator *>(&op)) {
    var_op->act(this);
  } else {
    VariableDefinition::traverse(op);
    Definition::apply(op);
  }
}

void
VariableDefinition::traverse(Ast::Visitor &visitor) const
{
  if (hasRule()) { rule->accept(visitor); }
}

void
VariableDefinition::traverse(Ast::Operator &op)
{
  if (hasRule()) { rule->apply(op); }
}
