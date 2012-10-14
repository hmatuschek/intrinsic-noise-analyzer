#include "reactioneditor.hh"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QCompleter>
#include "../tinytex/tinytex.hh"
#include "../models/scopeitemmodel.hh"


/* ********************************************************************************************* *
 * Implementation of ReactionEditor
 * ********************************************************************************************* */
ReactionEditor::ReactionEditor(iNA::Ast::Model &model, QWidget *parent)
  : QWizard(parent), _model(model)
{
  setWindowTitle(tr("Create new reaction"));

  // Assemble wizard pages:
  addPage(new ReactionEditorPage(this));
}


iNA::Ast::Model &
ReactionEditor::model()
{
  return _model;
}



/* ********************************************************************************************* *
 * Implementation of ReactionEditorPage
 * ********************************************************************************************* */
ReactionEditorPage::ReactionEditorPage(ReactionEditor *editor)
  : QWizardPage(editor), _model(editor->model())
{
  setTitle(tr("Define stoichiometry and kinetic law."));

  // The name field
  _name = new QLineEdit("reaction");
  _name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

  // The reactants stoichiometry
  _equation = new QLineEdit();
  _equation->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  QCompleter *completer = new QCompleter(ScopeItemModel::collectIdentifiers(_model, ScopeItemModel::SELECT_ALL));
  _equation->setCompleter(completer);
  // Get default color and define error color for equation editor:
  _default_background = _equation->palette().color(QPalette::Base);
  _error_background = Qt::red;

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
ReactionEditorPage::onMassActionToggled(bool state)
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
ReactionEditorPage::updateKineticLaw()
{
  // Process Equation:
  QList< QPair<int, QString> > reactants;
  QList< QPair<int, QString> > products;
  bool is_reversible = false;
  // Try to parse the expression
  if (! _parseEquation(_equation->text(), reactants, products, is_reversible)) {
    QPalette equation_palette = _equation->palette();
    equation_palette.setColor(QPalette::Base, _error_background);
    _equation->setPalette(equation_palette);
    return;
  }

  // If equation was parsed correctly -> set default background color:
  QPalette equation_palette = _equation->palette();
  equation_palette.setColor(QPalette::Base, _default_background);
  _equation->setPalette(equation_palette);

  // Do not update anything if autoupdate is not enabled.
  if (! _autoupdate->isChecked()) { return; }

  // Assemble formula
  MathFormula formula;

  // Handle reactants:
  if (is_reversible) {
    formula.appendItem(new MathSub(new MathText("k"), new MathText("fwd")));
  } else {
    formula.appendItem(new MathText("k"));
  }

  for (QList< QPair<int, QString> >::iterator item=reactants.begin();
       item != reactants.end(); item++)
  {
    formula.appendItem(new MathText(QChar(0x00B7)));
    formula.appendItem(assembleFactor(item->second, item->first));
  }

  // If reaction is reversible, include reverse rate
  if (is_reversible) {
    formula.appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));
    formula.appendItem(new MathText("-"));
    formula.appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));

    formula.appendItem(new MathSub(new MathText("k"), new MathText("rev")));
    for (QList< QPair<int, QString> >::iterator item=products.begin();
         item != products.end(); item++)
    {
      formula.appendItem(new MathText(QChar(0x00B7)));
      formula.appendItem(assembleFactor(item->second, item->first));
    }
  }

  _kineticLawFormula->setPixmap(formula.renderItem());
}


bool
ReactionEditorPage::_parseEquation(
  const QString &text, QList< QPair<int, QString> > &reactants,
  QList< QPair<int, QString> > &products, bool &reversible)
{
  // Check if reaction is reversible
  reversible = text.contains("=");

  // handle reversible reactions
  if (reversible) {
    QStringList parts = text.split("=");
    if (2 != parts.size()) { return false; }
    if (! _parseStoichiometry(parts.at(0).simplified(), reactants)) { return false; }
    return _parseStoichiometry(parts.at(1).simplified(), products);
  }

  // irreversible...

  QStringList parts = text.split("->");
  if (1 == parts.size()) {
    return _parseStoichiometry(parts.at(0).simplified(), reactants);
  }

  if (2 == parts.size()) {
    if (! _parseStoichiometry(parts.at(0).simplified(), reactants)) { return false; }
    return _parseStoichiometry(parts.at(1).simplified(), products);
  }

  return false;
}


bool
ReactionEditorPage::_parseStoichiometry(
  const QString &text, QList< QPair<int, QString> > &stoichiometry)
{
  QStringList sums = text.split('+');

  // For each product in sum
  for (QStringList::iterator prod=sums.begin(); prod!=sums.end(); prod++) {
    QStringList factors = prod->split('*');
    int factor = 1; QString species_id;

    if (1 == factors.size()) {
      // Unpack species
      species_id = factors.at(0);
      if (! _parseIdentifier(species_id)) { return 0; }
    } else if (2 == factors.size()) {
      bool success = true;
      // Unpack factor
      factor = factors.at(0).simplified().toInt(&success);
      if (! success) { return false; }
      if (factor <= 0) { return false; }
      // Unpack species id
      species_id = factors.at(1);
      if (! _parseIdentifier(species_id)) { return false; }
    } else {
      // Oops
      return false;
    }

    stoichiometry.push_back(QPair<int, QString>(factor, species_id));
  }

  return true;
}


bool
ReactionEditorPage::_parseIdentifier(QString &text)
{
  text = text.simplified();
  if (! QRegExp("[a-zA-Z_][a-zA-Z0-9_]*$").exactMatch(text)) { return false; }
  return true;
}


MathItem *
ReactionEditorPage::assembleFactor(QString &id, int exponent)
{
  MathItem    *name = assembleName(id);
  MathFormula *factor = new MathFormula();

  factor->appendItem(new MathText("["));
  factor->appendItem(name);
  factor->appendItem(new MathText("]"));

  if (exponent > 1) {
    return new MathSup(factor, new MathText(QString("%1").arg(exponent)));
  }

  return factor;
}


MathItem *
ReactionEditorPage::assembleCompartment()
{
  // if there is no compartment defined use \Omega
  if (0 == _model.numCompartments()) {
    return new MathText(QChar(0x03A9));
  }

  // Otherwise, get first compartment and take its name (if present)
  iNA::Ast::Compartment *comp = _model.getCompartment(0);
  if (comp->hasName()) { return TinyTex::parseQuoted(comp->getName().c_str()); }

  // If compartment has no name -> use identifier
  return new MathText(comp->getIdentifier().c_str());
}


MathItem *
ReactionEditorPage::assembleName(const QString &id)
{
  // Assemble name of the factor
  if (_model.hasVariable(id.toStdString())) {
    iNA::Ast::VariableDefinition *var = _model.getVariable(id.toStdString());
    if (iNA::Ast::Node::isSpecies(var) && var->hasName()) {
      return TinyTex::parseQuoted(var->getName());
    }
  }

  return new MathText(id);
}


bool
ReactionEditorPage::validatePage()
{
  // Check reaction equation:
  QList< QPair<int, QString> > reactants, products;
  bool is_reversible = false;
  // Try to parse the expression
  if (! _parseEquation(_equation->text(), reactants, products, is_reversible)) {
    return false;
  }

  /// @todo Check kinetic law!

  /// @todo Assemble equations & update scope.
  return true;
}



