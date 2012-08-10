#include "variableruledata.hh"


VariableRuleData::VariableRuleData(QObject *parent)
  : QObject(parent), _rule_kind(NO_RULE), _rule_expression()
{
  // pass...
}

VariableRuleData::VariableRuleData(RuleKind kind, const QString &expression, QObject *parent)
  : QObject(parent), _rule_kind(kind), _rule_expression(expression)
{
  // pass...
}

VariableRuleData::RuleKind VariableRuleData::ruleKind() const { return _rule_kind; }

void VariableRuleData::setRuleKind(RuleKind kind) { _rule_kind = kind; }

QString VariableRuleData::ruleExpression() const { return _rule_expression; }

void VariableRuleData::setRuleExpression(const QString &expression) { _rule_expression = expression; }
