#ifndef __INA_APP_VIEWS_VARIABLERULEEDITOR_HH__
#define __INA_APP_VIEWS_VARIABLERULEEDITOR_HH__

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include "../models/variableruledata.hh"


/** Trivial editor widget custom variable rules. */
class VariableRuleEditor : public QWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit VariableRuleEditor(QWidget *parent = 0);

  /** Returns the rule expression. */
  QString ruleExpression() const;
  /** Sets the rule expression. */
  void setRuleExpression(const QString &expr);

  /** Returns the rule kind. */
  VariableRuleData::RuleKind ruleKind() const;
  /** Resets the rule kind. */
  void setRuleKind(VariableRuleData::RuleKind kind);

private slots:
  /** Callback to set visibility of expression editor. */
  void _onKindSelected(int index);

private:
  /** The rule kind selector. */
  QComboBox *_kind_selector;
  /** The rule expression editor. */
  QLineEdit *_rule_edit;
};


#endif // __INA_APP_VIEWS_VARIABLERULEEDITOR_HH__
