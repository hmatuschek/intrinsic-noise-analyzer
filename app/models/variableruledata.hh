#ifndef __INA_APP_MODELS_VARIABLERULEDATA_HH__
#define __INA_APP_MODELS_VARIABLERULEDATA_HH__

#include <QObject>

/** This class packs the data custom a variable rule.
 * This class is used to transfer some incustommation beteen the SpeciesList model and the editor
 * of the species rule. */
class VariableRuleData : public QObject
{
  Q_OBJECT

public:
  /** Defines wich kind of rule is used. */
  typedef enum {
    NO_RULE,         ///< No rule at all.
    ASSIGNMENT_RULE, ///< An assignment rule.
    RATE_RULE        ///< A rate rule.
  } RuleKind;

public:
  /** Empty constructor. */
  explicit VariableRuleData(QObject *parent=0);
  /** Full constructor. */
  explicit VariableRuleData(RuleKind kind, const QString &expression, QObject *parent=0);

  /** Returns the rule kind. */
  RuleKind ruleKind() const;
  /** Resets the rule kind. */
  void setRuleKind(RuleKind kind);

  /** Returns the rule expression. */
  QString ruleExpression() const;
  /** Resets the rule expression. */
  void setRuleExpression(const QString &expression);

private:
  /** Holds the rule kind. */
  RuleKind _rule_kind;
  /** Holds the rule expression. */
  QString _rule_expression;
};


#endif // VARIABLERULEDATA_HH
