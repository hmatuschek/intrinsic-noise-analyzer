#include "variabledefinition.hh"

using namespace iNA::Ast;


VariableDefinition::VariableDefinition(Node::NodeType type, const std::string &id,
                                       bool is_const)
  : Definition(id, type), _has_value(false), _is_const(is_const), _symbol(id),
    _value(), _rule(0)
{
  // Done
}


VariableDefinition::VariableDefinition(Node::NodeType type, const std::string &id, GiNaC::ex value,
                                       const std::string &name, bool is_const)
  : Definition(id, name, type), _has_value(true), _is_const(is_const), _symbol(id),
    _value(value), _rule(0)
{
  // Done.
}


VariableDefinition::VariableDefinition(Node::NodeType type, const std::string &id, GiNaC::ex value, Rule *rule,
                                       bool is_const)
  : Definition(id, type), _has_value(true), _is_const(is_const), _symbol(id),
    _value(value), _rule(rule)
{
  // Done.
}


VariableDefinition::~VariableDefinition()
{
  // Free attached rule if there is one:
  if (0 != this->_rule) {
    delete this->_rule;
  }
}


void
VariableDefinition::setIdentifier(const std::string &id) {
  Definition::setIdentifier(id);
  _symbol.set_name(id);
}


bool VariableDefinition::isConst() const {
  return this->_is_const;
}

void VariableDefinition::setConst(bool is_const) {
  this->_is_const = is_const;
}

bool VariableDefinition::hasValue() const {
  return this->_has_value;
}

GiNaC::ex VariableDefinition::getValue() const {
  return this->_value;
}

void VariableDefinition::setValue(GiNaC::ex value) {
  this->_value = value;
  this->_has_value = true;
}

const GiNaC::symbol & VariableDefinition::getSymbol() const {
  return this->_symbol;
}

bool VariableDefinition::hasRule() const {
  return 0 != this->_rule;
}

Rule * VariableDefinition::getRule() {
  return this->_rule;
}

const Rule * VariableDefinition::getRule() const {
  return this->_rule;
}

void VariableDefinition::setRule(Rule *rule) {
  this->_rule = rule;
}


void
VariableDefinition::dump(std::ostream &str)
{
  if (this->_is_const) {
    str << "const " << this->getIdentifier();
  } else {
    str << "var " << this->getIdentifier();
  }

  if (this->hasValue()) {
    str << " = " << this->_value;
  }

  if (this->hasRule()) {
    this->_rule->dump(str);
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
  if (hasRule()) { _rule->accept(visitor); }
}

void
VariableDefinition::traverse(Ast::Operator &op)
{
  if (hasRule()) { _rule->apply(op); }
}
