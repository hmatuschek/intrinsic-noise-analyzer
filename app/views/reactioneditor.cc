#include "reactioneditor.hh"
#include <QFormLayout>
#include <QHBoxLayout>
#include "../tinytex/formula.hh"


ReactionCreator::ReactionCreator(QWidget *parent)
  : QDialog(parent)
{
  // General Dialog config:
  setWindowTitle(tr("Edit reaction"));
  setModal(true);

  // The name field
  _name = new QLineEdit("reaction");
  _name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

  // The reactants stoichiometry
  _equation = new QLineEdit();
  _equation->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

  // The kinetic law expression
  _autoupdate = new QCheckBox("mass action");
  _autoupdate->setChecked(true);
  _kineticLaw = new QStackedWidget();
  _kineticLawFormula = new QLabel();
  _kineticLaw->addWidget(_kineticLawFormula);
  _kineticLawEditor = new QLineEdit();
  _kineticLawEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  _kineticLawEditor->setEnabled(false);
  _kineticLaw->addWidget(_kineticLawEditor);
  _kineticLaw->setCurrentIndex(0);

  // Layout
  QHBoxLayout *kinlaw_layout = new QHBoxLayout();
  kinlaw_layout->addWidget(_kineticLaw);
  kinlaw_layout->addWidget(_autoupdate);

  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("Name"), _name);
  layout->addRow(tr("Reaction equation"), _equation);
  layout->addRow(tr("Kinetic law"), kinlaw_layout);
  setLayout(layout);

  // Delay timer
  _delayTimer = new QTimer(); _delayTimer->setSingleShot(true); _delayTimer->setInterval(500);

  // Connect all signals
  QObject::connect(_autoupdate, SIGNAL(toggled(bool)), this, SLOT(onMassActionToggled(bool)));
  QObject::connect(_equation, SIGNAL(editingFinished()), this, SLOT(updateKineticLaw()));
  QObject::connect(_equation, SIGNAL(textChanged(QString)), _delayTimer, SLOT(start()));
  QObject::connect(_delayTimer, SIGNAL(timeout()), this, SLOT(updateKineticLaw()));
}


void
ReactionCreator::onMassActionToggled(bool state)
{
  if (state) {
    _kineticLawEditor->setEnabled(false);
    _kineticLaw->setCurrentIndex(0);
    updateKineticLaw();
  } else {
    _kineticLawEditor->setEnabled(true);
    _kineticLaw->setCurrentIndex(1);
  }
}


void
ReactionCreator::updateKineticLaw()
{
  // Do not update anything if autoupdate is not enabled.
  if (! _autoupdate->isChecked()) { return; }

  // Process Equation:
  QList< QPair<int, QString> > reactants;
  QList< QPair<int, QString> > products;
  bool is_reversible = false;
  if (! parseEquation(_equation->text(), reactants, products, is_reversible)) { return; }

  // Assemble formula
  MathFormula formula;
  // Handle reactants:
  if (is_reversible) {
    formula.appendItem(
          new MathSub(new MathText("k"), new MathText("fwd")));
  } else {
    formula.appendItem(
          new MathText("k"));
  }

  for (QList< QPair<int, QString> >::iterator item=reactants.begin();
       item != reactants.end(); item++)
  {
    // \cdot
    formula.appendItem(new MathText(QChar(0x00B7)));
    // a factor
    if (1 == item->first) {
      formula.appendItem(new MathText(item->second));
    } else {
      formula.appendItem(
            new MathSup(
              new MathText(item->second), new MathText(QString("%1").arg(item->first))));
    }
  }

  if (is_reversible) {
    formula.appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));
    formula.appendItem(new MathText("-"));
    formula.appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));

    formula.appendItem(new MathSub(new MathText("k"), new MathText("rev")));
    for (QList< QPair<int, QString> >::iterator item=products.begin();
         item != products.end(); item++)
    {
      // \cdot
      formula.appendItem(new MathText(QChar(0x00B7)));
      // a factor
      if (1 == item->first) {
        formula.appendItem(new MathText(item->second));
      } else {
        formula.appendItem(
              new MathSup(
                new MathText(item->second), new MathText(QString("%1").arg(item->first))));
      }
    }
  }

  _kineticLawFormula->setPixmap(formula.renderItem());
}


bool
ReactionCreator::parseEquation(
  const QString &text, QList< QPair<int, QString> > &reactants,
  QList< QPair<int, QString> > &products, bool &reversible)
{
  // Check if reaction is reversible
  reversible = text.contains("=");

  // handle reversible reactions
  if (reversible) {
    QStringList parts = text.split("=");
    if (2 != parts.size()) { return false; }
    if (! parseStoichiometry(parts.at(0).simplified(), reactants)) { return false; }
    return parseStoichiometry(parts.at(1).simplified(), products);
  }

  // irreversible...

  QStringList parts = text.split("->");
  if (1 == parts.size()) {
    return parseStoichiometry(parts.at(0).simplified(), reactants);
  }

  if (2 == parts.size()) {
    if (! parseStoichiometry(parts.at(0).simplified(), reactants)) { return false; }
    return parseStoichiometry(parts.at(1).simplified(), products);
  }

  return false;
}


bool
ReactionCreator::parseStoichiometry(
  const QString &text, QList< QPair<int, QString> > &stoichiometry)
{
  QStringList sums = text.split('+');

  // For each product in sum
  for (QStringList::iterator prod=sums.begin(); prod!=sums.end(); prod++) {
    QStringList factors = prod->split('*');
    int factor = 1; QString species_id;

    if (1 == factors.size()) {
      // Unpack species
      species_id = factors.at(0).simplified();
    } else if (2 == factors.size()) {
      bool success = true;
      // Unpack factor
      factor = factors.at(0).simplified().toInt(&success);
      if (! success) { return false; }
      if (factor <= 0) { return false; }
      // Unpack species id
      species_id = factors.at(1).simplified();
      if (! QRegExp("[a-zA-Z_][a-zA-Z0-9_]*$").exactMatch(species_id)) { return false; }
    } else {
      // Oops
      return false;
    }

    stoichiometry.push_back(QPair<int, QString>(factor, species_id));
  }

  return true;
}
