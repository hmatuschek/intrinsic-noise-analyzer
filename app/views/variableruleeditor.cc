#include "variableruleeditor.hh"
#include <QHBoxLayout>


VariableRuleEditor::VariableRuleEditor(QWidget *parent)
  : QWidget(parent)
{
  _kind_selector = new QComboBox();
  _kind_selector->addItem("<none>", unsigned(VariableRuleData::NO_RULE));
  _kind_selector->addItem("x=", unsigned(VariableRuleData::ASSIGNMENT_RULE));
  _kind_selector->addItem("dx/dt=", unsigned(VariableRuleData::RATE_RULE));
  _kind_selector->setCurrentIndex(0);
  _kind_selector->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
  _kind_selector->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  _rule_edit = new QLineEdit();
  _rule_edit->setVisible(false);
  _rule_edit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

  QHBoxLayout *layout = new QHBoxLayout();
  layout->setMargin(0);
  layout->addWidget(_kind_selector);
  layout->addWidget(_rule_edit);
  setLayout(layout);

  // connect kind selection event:
  QObject::connect(_kind_selector, SIGNAL(currentIndexChanged(int)), this, SLOT(_onKindSelected(int)));
}


QString
VariableRuleEditor::ruleExpression() const {
  return _rule_edit->text();
}

void
VariableRuleEditor::setRuleExpression(const QString &expr) {
  _rule_edit->setText(expr);
}


VariableRuleData::RuleKind
VariableRuleEditor::ruleKind() const {
  if (0 == _kind_selector->currentIndex()) {
    return VariableRuleData::NO_RULE;
  } else if (1 == _kind_selector->currentIndex()) {
    return VariableRuleData::ASSIGNMENT_RULE;
  } else {
    return VariableRuleData::RATE_RULE;
  }
}


void
VariableRuleEditor::setRuleKind(VariableRuleData::RuleKind kind) {
  if (VariableRuleData::NO_RULE == kind) {
    _kind_selector->setCurrentIndex(0);
  } else if (VariableRuleData::ASSIGNMENT_RULE == kind) {
    _kind_selector->setCurrentIndex(1);
  } else if (VariableRuleData::RATE_RULE == kind) {
    _kind_selector->setCurrentIndex(2);
  }
}


void
VariableRuleEditor::_onKindSelected(int index)
{
  VariableRuleData::RuleKind kind
      = VariableRuleData::RuleKind(_kind_selector->itemData(index).toUInt());

  if (VariableRuleData::NO_RULE == kind) {
    _rule_edit->setVisible(false);
  } else {
    _rule_edit->setVisible(true);
  }
}

