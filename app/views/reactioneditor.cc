#include "reactioneditor.hh"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QCompleter>
#include "reactionequationrenderer.hh"
#include "../tinytex/tinytex.hh"
#include "../models/scopeitemmodel.hh"
#include <parser/exception.hh>


/* ********************************************************************************************* *
 * Implementation of ReactionEditorContext
 * ********************************************************************************************* */
ReactionEditorContext::ReactionEditorContext(iNA::Ast::Scope *root)
  : iNA::Parser::Expr::ScopeContext(root)
{
  // Pass...
}


GiNaC::symbol
ReactionEditorContext::resolve(const std::string &identifier)
{
  try {
    return iNA::Parser::Expr::ScopeContext::resolve(identifier);
  } catch (iNA::Parser::ParserError &err) {
    // pass...
  }

  _undefined_symbols[identifier] = GiNaC::symbol(identifier);
  return _undefined_symbols[identifier];
}

const std::map<std::string, GiNaC::symbol> &
ReactionEditorContext::undefinedSymbols() const {
  return _undefined_symbols;
}



/* ********************************************************************************************* *
 * Implementation of ReactionEditor
 * ********************************************************************************************* */
ReactionEditor::ReactionEditor(iNA::Ast::Model &model, QWidget *parent)
  : QWizard(parent), _model(model), _current_reaction_scope(0)
{
  setWindowTitle(tr("Create new reaction"));

  // Assemble wizard pages:
  addPage(new ReactionEditorPage(this));
  addPage(new ReactionEditorSummaryPage(this));
}


iNA::Ast::Model &
ReactionEditor::model()
{
  return _model;
}


iNA::Ast::Reaction *
ReactionEditor::reaction() {
  if (0 == _current_reaction_scope) { return 0; }
  for (iNA::Ast::Scope::iterator item = _current_reaction_scope->begin();
       item != _current_reaction_scope->end(); item++)
  {
    if (iNA::Ast::Node::isReactionDefinition(*item)) {
      return static_cast<iNA::Ast::Reaction *>(*item);
    }
  }

  return 0;
}


void
ReactionEditor::setReactionScope(iNA::Ast::Scope *reaction_scope)
{
  if (0 != _current_reaction_scope) {
    delete _current_reaction_scope;
  }

  _current_reaction_scope = reaction_scope;
}


void
ReactionEditor::commitReactionScope()
{
  std::list<iNA::Ast::Definition *> definitions;
  // Add all definitions in the current reaction scope to the model:
  for (iNA::Ast::Scope::iterator item=_current_reaction_scope->begin();
       item!=_current_reaction_scope->end(); item++) {
    _model.addDefinition(*item);
    definitions.push_back(*item);
  }

  // clear current reaction scope:
  for (std::list<iNA::Ast::Definition *>::iterator item = definitions.begin();
       item != definitions.end(); item++) {
    _current_reaction_scope->remDefinition(*item);
  }

  // Delete scope:
  delete _current_reaction_scope;
  _current_reaction_scope = 0;
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
  QObject::connect(_autoupdate, SIGNAL(toggled(bool)), this, SLOT(_onMassActionToggled(bool)));
  QObject::connect(_equation, SIGNAL(editingFinished()), this, SLOT(_updateKineticLaw()));
  QObject::connect(_equation, SIGNAL(textChanged(QString)), _delayTimer, SLOT(start()));
  QObject::connect(_delayTimer, SIGNAL(timeout()), this, SLOT(_updateKineticLaw()));
}


void
ReactionEditorPage::_onMassActionToggled(bool state)
{
  if (state) {
    _kineticLawEditor->setEnabled(false);
    _kineticLaw->setCurrentIndex(0);
    _updateKineticLaw();
  } else {
    _kineticLawEditor->setEnabled(true);
    _kineticLaw->setCurrentIndex(1);
  }
}


void
ReactionEditorPage::_updateKineticLaw()
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
    formula.appendItem(_assembleFactor(item->second, item->first));
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
      formula.appendItem(_assembleFactor(item->second, item->first));
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
ReactionEditorPage::_assembleFactor(QString &id, int exponent)
{
  MathItem    *name = _assembleName(id);
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
ReactionEditorPage::_assembleCompartment()
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
ReactionEditorPage::_assembleName(const QString &id)
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


void
ReactionEditorPage::_defineUnknownSpecies(QList<QPair<int, QString> > &reactants,
                                          QList<QPair<int, QString> > &products,
                                          iNA::Ast::Scope *scope)
{
  // Process reactants...
  for (QList< QPair<int, QString> >::iterator item = reactants.begin(); item!=reactants.end(); item++) {
    // If identifier names a species -> skip
    if (scope->hasVariable(item->second.toStdString()) &&
        iNA::Ast::Node::isSpecies(scope->getVariable(item->second.toStdString()))) {
      continue;
    }

    // if identifier names any other definition -> error.
    if (scope->hasDefinition(item->second.toStdString())) {
      iNA::SymbolError err;
      err << "Can not define identifier " << item->second.toStdString() << " as a species, "
          << " identifier already used by an other definition.";
      throw err;
    }

    if (0 == _model.numCompartments()) {
      iNA::InternalError err;
      err << "Can not define species with identifier " << item->second.toStdString()
          << ", no compartment defined.";
      throw err;
    }

    // identifier not in use -> define species with this identifier.
    iNA::Ast::Species *new_species = new iNA::Ast::Species(
          item->second.toStdString(), _model.getCompartment(0), false);
    scope->addDefinition(new_species);
  }

  // Process products...
  for (QList< QPair<int, QString> >::iterator item = products.begin(); item!=products.end(); item++) {
    // If identifier names a species -> skip
    if (scope->hasVariable(item->second.toStdString()) &&
        iNA::Ast::Node::isSpecies(scope->getVariable(item->second.toStdString()))) {
      continue;
    }

    // if identifier names any other definition -> error.
    if (scope->hasDefinition(item->second.toStdString())) {
      iNA::SymbolError err;
      err << "Can not define identifier " << item->second.toStdString() << " as a species, "
          << " identifier already used by an other definition.";
      throw err;
    }

    if (0 == _model.numCompartments()) {
      iNA::InternalError err;
      err << "Can not define species with identifier " << item->second.toStdString()
          << ", no compartment defined.";
      throw err;
    }

    // identifier not in use -> define species with this identifier.
    iNA::Ast::Species *new_species = new iNA::Ast::Species(
          item->second.toStdString(), _model.getCompartment(0), false);
    scope->addDefinition(new_species);
  }
}


iNA::Ast::Reaction *
ReactionEditorPage::_createReaction(const QString &name, QList<QPair<int, QString> > &reactants,
                                    QList<QPair<int, QString> > &products, bool is_reversible,
                                    iNA::Ast::Scope *scope)
{
  // Create unique identifier for reaction:
  std::string identifier = name.simplified().toStdString();
  if (0 == identifier.length()) { identifier = "reaction"; }
  identifier = scope->getRootScope()->getNewIdentifier(identifier);

  // Create reaction and add to scope:
  iNA::Ast::Reaction *new_reaction = new iNA::Ast::Reaction(
        identifier, new iNA::Ast::KineticLaw(0), is_reversible);
  scope->addDefinition(new_reaction);

  // Create reactants:
  for (QList< QPair<int, QString> >::iterator item=reactants.begin(); item!=reactants.end(); item++) {
    iNA::Ast::Definition *def = scope->getDefinition(item->second.toStdString());
    if (! iNA::Ast::Node::isSpecies(def)) {
      iNA::InternalError err;
      err << "Identifier " << item->second.toStdString()
          << " in reactants list does not name a species.";
      throw err;
    }

    new_reaction->addReactantStoichiometry(static_cast<iNA::Ast::Species *>(def), item->first);
  }

  // Create products:
  for (QList< QPair<int, QString> >::iterator item=products.begin(); item!=products.end(); item++) {
    iNA::Ast::Definition *def = scope->getDefinition(item->second.toStdString());
    if (! iNA::Ast::Node::isSpecies(def)) {
      iNA::InternalError err;
      err << "Identifier " << item->second.toStdString()
          << " in products list does not name a species.";
      throw err;
    }

    new_reaction->addProductStoichiometry(static_cast<iNA::Ast::Species *>(def), item->first);
  }

  return new_reaction;
}


void
ReactionEditorPage::_createKineticLaw(iNA::Ast::Reaction *reaction)
{
  if (_autoupdate->isChecked()) {
    _createMassActionKineticLaw(reaction);
  } else {
    _parseAndCreateKineticLaw(reaction);
  }
}


void
ReactionEditorPage::_createMassActionKineticLaw(iNA::Ast::Reaction *reaction)
{
  iNA::Ast::Parameter *k_fwd=0, *k_rev=0;
  iNA::Ast::KineticLaw *law = reaction->getKineticLaw();

  // Define local parameters:
  if (!reaction->isReversible()) {
    k_fwd = new iNA::Ast::Parameter(law->getNewIdentifier("k"), iNA::Ast::Unit::dimensionless(), true);
    k_fwd->setName("k"); k_fwd->setValue(0);
    law->addDefinition(k_fwd);
  } else {
    k_fwd = new iNA::Ast::Parameter(law->getNewIdentifier("k_fwd"), iNA::Ast::Unit::dimensionless(), true);
    k_fwd->setName("$k_fwd$"); k_fwd->setValue(0);
    law->addDefinition(k_fwd);
    k_rev = new iNA::Ast::Parameter(law->getNewIdentifier("k_rev"), iNA::Ast::Unit::dimensionless(), true);
    k_rev->setName("$k_rev$"); k_rev->setValue(0);
    law->addDefinition(k_rev);
  }

  GiNaC::ex factor_fwd = k_fwd->getSymbol();
  GiNaC::ex factor_rev = 0;
  // Iterate over reactants:
  for (iNA::Ast::Reaction::iterator reac=reaction->reactantsBegin(); reac!=reaction->reactantsEnd(); reac++) {
    if (_model.speciesHasSubstanceUnits()) {
      factor_fwd *= GiNaC::pow(reac->first->getSymbol()/reac->first->getCompartment()->getSymbol(), reac->second);
    } else {
      factor_fwd *= GiNaC::pow(reac->first->getSymbol(), reac->second);
    }
  }

  // Iterate over products if reaction is reversible:
  if (reaction->isReversible()) {
    factor_rev = k_rev->getSymbol();
    // Iterate over reactants:
    for (iNA::Ast::Reaction::iterator prod=reaction->productsBegin(); prod!=reaction->productsEnd(); prod++) {
      if (_model.speciesHasSubstanceUnits()) {
        factor_rev *= GiNaC::pow(prod->first->getSymbol()/prod->first->getCompartment()->getSymbol(), prod->second);
      } else {
        factor_rev *= GiNaC::pow(prod->first->getSymbol(), prod->second);
      }
    }
  }

  // Assemble and assign kinetic law:
  law->setRateLaw(factor_fwd - factor_rev);
}


void
ReactionEditorPage::_parseAndCreateKineticLaw(iNA::Ast::Reaction *reaction)
{
  // Parse kinetic law expression:
  iNA::Ast::KineticLaw *law = reaction->getKineticLaw();
  ReactionEditorContext ctx(law);
  GiNaC::ex rate_law = iNA::Parser::Expr::parseExpression(
        _kineticLawFormula->text().toStdString(), ctx);

  // get symbols of unresolved variables (parameters):
  GiNaC::exmap substitution;
  std::map<std::string, GiNaC::symbol> unresolved_symbols = ctx.undefinedSymbols();
  for (std::map<std::string, GiNaC::symbol>::iterator item=unresolved_symbols.begin();
       item != unresolved_symbols.end(); item++)
  {
    // Define a parameter for the unresolved symbol:
    iNA::Ast::Parameter *param = new iNA::Ast::Parameter(item->first, iNA::Ast::Unit::dimensionless(), true);
    law->addDefinition(param);
    // Add subst:
    substitution[item->second] = param->getSymbol();
  }

  // Perform parameter substitution and define kinetic law:
  law->setRateLaw(rate_law.subs(substitution));
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

  // Check kinetic law (if defined by user):
  if (! _autoupdate->isChecked()) {
    ReactionEditorContext ctx(&_model);
    try { iNA::Parser::Expr::parseExpression(_kineticLawFormula->text().toStdString(), ctx); }
    catch (iNA::Parser::ParserError &err) { return false; }
  }

  // Construct new scope, holding all undefined variables:
  iNA::Ast::Scope *reaction_scope = new iNA::Ast::Scope(&_model);
  _defineUnknownSpecies(reactants, products, reaction_scope);

  // Create reaction with empty kinetic law:
  iNA::Ast::Reaction *new_reaction = _createReaction(
        _name->text(), reactants, products, is_reversible, reaction_scope);

  // Create kinetic law for that reaction
  _createKineticLaw(new_reaction);

  // Store reaction equation:
  ReactionEditor *editor = static_cast<ReactionEditor *>(wizard());
  editor->setReactionScope(reaction_scope);

  return true;
}



/* ********************************************************************************************* *
 * Implementation of ReactionEditorSummaryPage
 * ********************************************************************************************* */
ReactionEditorSummaryPage::ReactionEditorSummaryPage(ReactionEditor *wizard)
  : QWizardPage(wizard), _model(wizard->model())
{
  setTitle(tr("Summary"));

  // Reaction preview
  _reaction_preview = new QLabel();

  // Assemble layout
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_reaction_preview);
  setLayout(layout);
}


void
ReactionEditorSummaryPage::initializePage()
{
  ReactionEditor *editor = static_cast<ReactionEditor *>(wizard());
  // Render equation
  MathContext ctx; ctx.setFontSize(ctx.fontSize()*0.66);
  _reaction_preview->setPixmap(ReactionEquationRenderer::renderReaction(editor->reaction()));
}
