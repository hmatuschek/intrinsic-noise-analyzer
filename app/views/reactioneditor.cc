#include "reactioneditor.hh"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QCompleter>
#include <QGroupBox>

#include "reactionequationrenderer.hh"
#include "../tinytex/tinytex.hh"
#include "../models/scopeitemmodel.hh"

#include <ast/unitconverter.hh>
#include <parser/exception.hh>
#include <utils/logger.hh>



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


iNA::Ast::Scope *
ReactionEditor::reactionScope() {
  return _current_reaction_scope;
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
  _kinetic_law_type = new QComboBox();
  _kinetic_law_type->addItem("Mass action - single compartment", (unsigned) MASSACTION_SINGLE);
  _kinetic_law_type->addItem("Mass action - multi compartment", (unsigned) MASSACTION_MULTI);
  _kinetic_law_type->addItem("User defined", (unsigned) USER_DEFINED);
  _kinetic_law_type->setCurrentIndex(0);

  _kineticLaw = new QStackedWidget();
  _kineticLawFormula = new QLabel();
  _kineticLaw->addWidget(_kineticLawFormula);
  _kineticLawEditor = new QLineEdit();
  _kineticLawEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  _kineticLawEditor->setEnabled(false);
  _kineticLaw->addWidget(_kineticLawEditor);
  _kineticLaw->setCurrentIndex(0);

  // Layout
  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("Name"), _name);
  layout->addRow(tr("Reaction equation"), _equation);
  layout->addRow(tr("Type"), _kinetic_law_type);
  layout->addRow(tr("Kinetic law"), _kineticLaw);
  setLayout(layout);

  // Delay timer
  _delayTimer = new QTimer(); _delayTimer->setSingleShot(true); _delayTimer->setInterval(500);

  // Connect all signals
  QObject::connect(_kinetic_law_type, SIGNAL(currentIndexChanged(int)), this, SLOT(_onKineticLawTypeChanged(int)));
  QObject::connect(_equation, SIGNAL(editingFinished()), this, SLOT(_updateKineticLaw()));
  QObject::connect(_equation, SIGNAL(textChanged(QString)), _delayTimer, SLOT(start()));
  QObject::connect(_delayTimer, SIGNAL(timeout()), this, SLOT(_updateKineticLaw()));
}


ReactionEditorPage::KineticLawType
ReactionEditorPage::kineticLawType() const {
  return (KineticLawType)(_kinetic_law_type->itemData(_kinetic_law_type->currentIndex()).toUInt());
}

void
ReactionEditorPage::setKineticLawType(KineticLawType type)
{
  QList< QPair<int, QString> > reactants, products;
  bool is_reversible=false, is_valid=false, is_multicompartment=false;

  is_valid = _parseEquation(_equation->text(), reactants, products, is_reversible);
  is_multicompartment = (1 != _collectCompartments(reactants, products).size());

  // If mass action single compartment is selected, but reaction is multicompartment:
  if (is_valid && is_multicompartment && (MASSACTION_SINGLE == type)) {
    type = MASSACTION_MULTI;
  }

  switch (type) {
  case MASSACTION_SINGLE: _kinetic_law_type->setCurrentIndex(0); break;
  case MASSACTION_MULTI: _kinetic_law_type->setCurrentIndex(1); break;
  case USER_DEFINED: _kinetic_law_type->setCurrentIndex(2); break;
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
  if (USER_DEFINED == kineticLawType()) { return; }

  // If mass action single compartment is selected but reaction is multicompartment:
  if ((MASSACTION_SINGLE == kineticLawType()) && 1<_collectCompartments(reactants, products).size()) {
    setKineticLawType(MASSACTION_MULTI);
  }

  // Assemble formula
  MathItem *formula = _renderKineticLaw(is_reversible, reactants, products);
  _kineticLawFormula->setPixmap(formula->renderItem());
  delete formula;
}


void
ReactionEditorPage::_onKineticLawTypeChanged(int index) {
  switch (kineticLawType()) {
  case MASSACTION_SINGLE:
  case MASSACTION_MULTI:
    _kineticLawEditor->setEnabled(false);
    _kineticLaw->setCurrentIndex(0);
    _updateKineticLaw();
    break;

  case USER_DEFINED:
    _kineticLawEditor->setEnabled(true);
    _kineticLaw->setCurrentIndex(1);
    break;
  }
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
    if (! _parseStoichiometry(parts.at(1).simplified(), products)) { return false; }
    if ((0 == reactants.size()) && (0 == products.size())) { return false; }

    return true;
  }

  // irreversible...
  QStringList parts = text.split("->");
  if (2 != parts.size()) { return false; }

  if (! _parseStoichiometry(parts.at(0).simplified(), reactants)) { return false; }
  if (! _parseStoichiometry(parts.at(1).simplified(), products)) { return false; }
  if ((0==reactants.size()) && (0==products.size())) { return false; }

  return true;
}


bool
ReactionEditorPage::_parseStoichiometry(
  const QString &text, QList< QPair<int, QString> > &stoichiometry)
{
  QStringList sums = text.split('+');

  // Check for empty stoichiometry:
  if (1==sums.size() && (0 == sums.at(0).size())) { return true; }

  // For each product in sum
  for (QStringList::iterator prod=sums.begin(); prod!=sums.end(); prod++) {
    QStringList factors = prod->split('*');
    int factor = 1; QString species_id;

    if (1 == factors.size()) {
      // Unpack species
      species_id = factors.at(0);
      if (! _parseIdentifier(species_id)) { return false; }
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


std::set<iNA::Ast::Compartment *>
ReactionEditorPage::_collectCompartments(QList<QPair<int, QString> > &reactants,
                                         QList<QPair<int, QString> > &products)
{
  std::set<iNA::Ast::Compartment *> compartments;
  // Handle list of reactants:
  for (QList< QPair<int, QString> >::iterator item=reactants.begin(); item!=reactants.end(); item++)
  {
    if (! _model.hasSpecies(item->second.toStdString())) {
      compartments.insert(_model.getCompartment(0));
    } else {
      compartments.insert(_model.getSpecies(item->second.toStdString())->getCompartment());
    }
  }
  // Handle list of products
  for (QList< QPair<int, QString> >::iterator item=products.begin(); item!=products.end(); item++)
  {
    if (! _model.hasSpecies(item->second.toStdString())) {
      compartments.insert(_model.getCompartment(0));
    } else {
      compartments.insert(_model.getSpecies(item->second.toStdString())->getCompartment());
    }
  }

  return compartments;
}



MathItem *
ReactionEditorPage::_renderKineticLaw(bool is_reversible, QList<QPair<int, QString> > &reactants,
                                      QList<QPair<int, QString> > &products)
{
  std::set<iNA::Ast::Compartment *> compartments = _collectCompartments(reactants, products);


  MathFormula *formula = new MathFormula();

  // Handle reactants:
  if (is_reversible) {
    formula->appendItem(new MathSub(new MathText("k"), new MathText("fwd")));
  } else {
    formula->appendItem(new MathText("k"));
  }

  if (MASSACTION_SINGLE == kineticLawType()) {
    formula->appendItem(new MathText(QChar(0x00B7)));
    formula->appendItem(_renderCompartment(*(compartments.begin())));
  }

  for (QList< QPair<int, QString> >::iterator item=reactants.begin(); item != reactants.end(); item++)
  {
    formula->appendItem(new MathText(QChar(0x00B7)));
    formula->appendItem(_renderFactor(item->second, item->first));
  }

  // If reaction is reversible, include reverse rate
  if (is_reversible) {
    formula->appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));
    formula->appendItem(new MathText("-"));
    formula->appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));

    formula->appendItem(new MathSub(new MathText("k"), new MathText("rev")));

    if (MASSACTION_SINGLE == kineticLawType()) {
      formula->appendItem(new MathText(QChar(0x00B7)));
      formula->appendItem(_renderCompartment(*(compartments.begin())));
    }

    for (QList< QPair<int, QString> >::iterator item=products.begin(); item != products.end(); item++)
    {
      formula->appendItem(new MathText(QChar(0x00B7)));
      formula->appendItem(_renderFactor(item->second, item->first));
    }
  }

  return formula;
}


MathItem *
ReactionEditorPage::_renderFactor(QString &id, int exponent)
{
  MathFormula *name = new MathFormula();
  name->appendItem(new MathText("["));
  name->appendItem(_renderName(id));
  name->appendItem(new MathText("]"));

  MathFormula *factor = new MathFormula();

  if (MASSACTION_MULTI == kineticLawType()) {
    if (1 == exponent) {
      factor->appendItem(name->copy());
    } else {
      factor->appendItem(new MathSup(name->copy(), new MathText(QString("%1").arg(exponent))));
    }
  } else if (MASSACTION_SINGLE == kineticLawType()) {
    factor->appendItem(name->copy());
    for (int i=1; i<exponent; i++) {
      MathFormula *term = new MathFormula();
      term->appendItem(name->copy());
      term->appendItem(new MathText("-"));
      if (i > 1) {
        term->appendItem(new MathText(QString("%1").arg(i)));
        term->appendItem(new MathText(QChar(0x00B7)));
      }
      term->appendItem(new MathSup(_renderCompartmentOf(id), new MathText("-1")));
      factor->appendItem(new MathBlock(term, new MathText("("), new MathText(")")));
    }
  }

  delete name;
  return factor;
}


MathItem *
ReactionEditorPage::_renderCompartmentOf(const QString &id)
{
  // if there is no compartment defined use \Omega
  if (0 == _model.numCompartments()) {
    return new MathText(QChar(0x03A9));
  }

  if (! _model.hasSpecies(id.toStdString())) {
    return _renderCompartment(_model.getCompartment(0));
  }

  return _renderCompartment(_model.getSpecies(id.toStdString())->getCompartment());
}


MathItem *
ReactionEditorPage::_renderCompartment(iNA::Ast::Compartment *compartment)
{
  // If compartment has no name -> use identifier
  if (compartment->hasName()) { return TinyTex::parseQuoted(compartment->getName().c_str()); }
  return new MathText(compartment->getIdentifier().c_str());
}


MathItem *
ReactionEditorPage::_renderName(const QString &id)
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
  switch (kineticLawType()) {
  case MASSACTION_SINGLE:
  case MASSACTION_MULTI:
    _createMAKineticLaw(reaction);
    break;

  case USER_DEFINED:
    _parseAndCreateKineticLaw(reaction);
    break;
  }
}


void
ReactionEditorPage::_createMAKineticLaw(iNA::Ast::Reaction *reaction)
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

  GiNaC::ex factor_fwd = 0;
  GiNaC::ex factor_rev = 0;

  factor_fwd = k_fwd->getSymbol();
  if (reaction->isReversible()) {
    factor_rev = k_rev->getSymbol();
  }

  if (MASSACTION_SINGLE == kineticLawType()) {
    iNA::Ast::Compartment *compartment=0;
    // Obtain compartment of reaction:
    for (iNA::Ast::Reaction::iterator reac=reaction->reactantsBegin(); reac != reaction->reactantsEnd(); reac++) {
      compartment = reac->first->getCompartment(); break;
    }
    for (iNA::Ast::Reaction::iterator prod=reaction->productsBegin(); prod != reaction->productsEnd(); prod++) {
      compartment = prod->first->getCompartment(); break;
    }

    factor_fwd *= compartment->getSymbol();
    factor_rev *= compartment->getSymbol();
  }

  // Iterate over reactants:
  for (iNA::Ast::Reaction::iterator reac=reaction->reactantsBegin(); reac!=reaction->reactantsEnd(); reac++) {
    factor_fwd *= _createMAFactor(reac->first, reac->second);
  }

  // Iterate over products if reaction is reversible:
  if (reaction->isReversible()) {
    // Iterate over reactants:
    for (iNA::Ast::Reaction::iterator prod=reaction->productsBegin(); prod!=reaction->productsEnd(); prod++) {
      factor_rev *= _createMAFactor(prod->first, prod->second);
    }
  }

  // Assemble and assign kinetic law:
  law->setRateLaw(factor_fwd - factor_rev);
}


GiNaC::ex
ReactionEditorPage::_createMAFactor(iNA::Ast::Species *species, GiNaC::ex stoichiometry)
{
  // Check type of stoichiometry
  if (! GiNaC::is_a<GiNaC::numeric>(stoichiometry)) {
    iNA::InternalError err;
    err << "Can not assemble kinetic law for stoichiometry " << stoichiometry
        << ": is not an numeric value.";
    throw err;
  }

  GiNaC::numeric value = GiNaC::ex_to<GiNaC::numeric>(stoichiometry);
  if (! value.is_pos_integer()) {
    iNA::InternalError err;
    err << "Can not assemble kinetic law for stoichiometry " << stoichiometry
        << ": is not a positive integer.";
    throw err;
  }

  // dispatch
  if (MASSACTION_SINGLE == kineticLawType()) {
    return _createMASingleFactor(species, value.to_int());
  }
  return _createMAMultiFactor(species, value.to_int());
}


GiNaC::ex
ReactionEditorPage::_createMASingleFactor(iNA::Ast::Species *species, int stoichiometry)
{
  if (0 >= stoichiometry) { return 1; }

  // Get species "symbol"
  GiNaC::ex species_expr = species->getSymbol();
  // If species are defined in substance units:
  if (_model.speciesHasSubstanceUnits()) {
    species_expr /= species->getCompartment()->getSymbol();
  }
  // If substance units are not item
  species_expr *= iNA::Ast::UnitConverter::conversionFactor(
        iNA::Ast::ScaledBaseUnit(iNA::Ast::ScaledBaseUnit::ITEM, 1, 0, 1),
        _model.getSubstanceUnit());

  GiNaC::ex factor=species_expr;
  for (int i=1; i<stoichiometry; i++) {
    factor *= (species_expr-i/species->getCompartment()->getSymbol());
  }

  return factor;
}


GiNaC::ex
ReactionEditorPage::_createMAMultiFactor(iNA::Ast::Species *species, int stoichiometry)
{
  if (0 >= stoichiometry) { return 1; }

  // Get species "symbol"
  GiNaC::ex species_expr = species->getSymbol();
  // If species are defined in substance units:
  if (_model.speciesHasSubstanceUnits()) {
    species_expr /= species->getCompartment()->getSymbol();
  }

  return GiNaC::pow(species_expr, stoichiometry);
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
  if (USER_DEFINED == kineticLawType()) {
    ReactionEditorContext ctx(&_model);
    try {
      iNA::Parser::Expr::parseExpression(_kineticLawFormula->text().toStdString(), ctx);
    } catch (iNA::Parser::ParserError &err) {
      iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::INFO);
      message << "Invalid kinetic law expression: " << err.what();
      iNA::Utils::Logger::get().log(message);
      return false;
    }
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
  QGroupBox *prev_box = new QGroupBox(tr("Reaction Preview"));
  _reaction_preview = new QLabel();
  QVBoxLayout *prev_box_layout = new QVBoxLayout();
  prev_box_layout->addWidget(_reaction_preview);
  prev_box->setLayout(prev_box_layout);

  QFormLayout *species_layout = new QFormLayout();
  _created_species = new QLabel();
  species_layout->addRow(tr("Created species"), _created_species);

  // Assemble layout
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(prev_box);
  layout->addLayout(species_layout);
  setLayout(layout);
}


void
ReactionEditorSummaryPage::initializePage()
{
  ReactionEditor *editor = static_cast<ReactionEditor *>(wizard());

  // Render equation
  MathContext ctx; ctx.setFontSize(ctx.fontSize()*0.66);
  _reaction_preview->setPixmap(ReactionEquationRenderer::renderReaction(editor->reaction()));

  // Assemble list of created species:
  QStringList created_species;
  iNA::Ast::Scope *scope = editor->reactionScope();
  for (iNA::Ast::Scope::iterator item=scope->begin(); item!=scope->end(); item++) {
    if (! iNA::Ast::Node::isSpecies(*item)) { continue; }
    created_species.append((*item)->getIdentifier().c_str());
  }

  if (0 == created_species.size()) {
    _created_species->setText(tr("<none>"));
  } else {
    _created_species->setText(created_species.join(", "));
  }
}
