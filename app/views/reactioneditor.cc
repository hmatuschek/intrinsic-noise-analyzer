#include "reactioneditor.hh"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QCompleter>
#include <QGroupBox>
#include <QMessageBox>
#include <QDebug>

#include "reactionequationrenderer.hh"
#include "../tinytex/tinytex.hh"
#include "../tinytex/ginac2formula.hh"
#include "../models/scopeitemmodel.hh"
#include "expressioneditor.hh"

#include <ast/unitconverter.hh>
#include <ast/identifier.hh>
#include <parser/exception.hh>
#include <utils/logger.hh>



/* ********************************************************************************************* *
 * Implementation of ReactionEditorContext
 * ********************************************************************************************* */
ReactionEditorContext::ReactionEditorContext(iNA::Ast::Model *model, iNA::Ast::Scope *root)
  : iNA::Parser::Expr::ScopeContext(0 == root ? model : root), _model(model)
{
  // If there is no compartment defined in the model -> create a new symbol for it and
  // find an unique ID.
  if (0 == _model->numCompartments()) {
    _compartment_id = _model->getNewIdentifier("compartment");
    _compartment_symbol = GiNaC::symbol(_compartment_id);
    _compartment_undefined = true;
  } else {
    _compartment_id = _model->getCompartment(0)->getIdentifier();
    _compartment_symbol = _model->getCompartment(0)->getSymbol();
    _compartment_undefined = false;
  }
}


GiNaC::symbol
ReactionEditorContext::resolve(const std::string &identifier)
{
  // First, try to resolve id in local parameters
  if (0 != _param_symbols.count(identifier)) { return _param_symbols[identifier]; }
  // then in species ...
  if (0 != _species_symbols.count(identifier)) { return _species_symbols[identifier]; }
  // then in compartment ...
  if (_compartment_id == identifier) { return _compartment_symbol; }

  // Then, try to resolve symbol in parent scope
  try {
    return iNA::Parser::Expr::ScopeContext::resolve(identifier);
  } catch (iNA::Exception &err) {
    // pass...
  }

  // If symbol is not defined at all, define it as a local parameter
  _param_symbols[identifier] = GiNaC::symbol(identifier);

  // done...
  return _param_symbols[identifier];
}


std::string
ReactionEditorContext::identifier(GiNaC::symbol symbol)
{
  // First, try to resolve symbol in local parameters
  if (0 != _param_ids.count(symbol)) { return _param_ids[symbol]; }
  if (0 != _species_ids.count(symbol)) { return _species_ids[symbol]; }
  if (_compartment_symbol == symbol) { return _compartment_id; }

  // If symbol is not a local parameter, try to resolve it in parent scope
  return iNA::Parser::Expr::ScopeContext::identifier(symbol);
}

const std::map<std::string, GiNaC::symbol> &
ReactionEditorContext::undefinedParameters() const {
  return _param_symbols;
}

GiNaC::symbol
ReactionEditorContext::getOrCreateLocalParameter(const std::string &id) {
  if (0 != _param_symbols.count(id)) { return _param_symbols[id]; }
  if (_scope->hasDefinition(id, true) && iNA::Ast::Node::isParameter(_scope->getDefinition(id))) {
    return _scope->getVariable(id)->getSymbol();
  }
  _param_symbols[id] = GiNaC::symbol(id);
  _param_ids[_param_symbols[id]] = id;
  return _param_symbols[id];
}

bool
ReactionEditorContext::hasIdentifier(const std::string &id) const {
  if (0 != _param_symbols.count(id)) { return true; }
  if (0 != _species_symbols.count(id)) { return true; }
  if (_compartment_id == id) { return true; }
  return _scope->hasDefinition(id);
}

const std::map<std::string, GiNaC::symbol> &
ReactionEditorContext::undefinedSpecies() const {
  return _species_symbols;
}

bool
ReactionEditorContext::hasSpecies(const std::string &id) const {
  return 0 != _species_symbols.count(id);
}

GiNaC::symbol
ReactionEditorContext::getOrCreateSpecies(const std::string &id) {
  if (hasSpecies(id)) { return _species_symbols[id]; }
  if (_model->hasSpecies(id)) { return _model->getSpecies(id)->getSymbol(); }
  _species_symbols[id] = GiNaC::symbol(id);
  _species_ids[_species_symbols[id]] = id;
  return _species_symbols[id];
}

const GiNaC::symbol &
ReactionEditorContext::compartmentSymbol() const{
  return _compartment_symbol;
}

const std::string &
ReactionEditorContext::compartmentIdentifier() const {
  return _compartment_id;
}

bool
ReactionEditorContext::compartmentIsUndefined() const {
  return _compartment_undefined;
}

bool
ReactionEditorContext::hasConcentrationUnits() const  {
  return !(_model->speciesHaveSubstanceUnits());
}

void
ReactionEditorContext::reset() {
  _param_ids.clear();
  _param_symbols.clear();
  _species_ids.clear();
  _species_symbols.clear();
}


/* ********************************************************************************************* *
 * Implementation of ReactionEditorDisplayContext
 * ********************************************************************************************* */
ReactionEditorDisplayContext::ReactionEditorDisplayContext(ReactionEditorContext &context)
  : _context(context)
{
  // pass...
}

GiNaC::symbol
ReactionEditorDisplayContext::resolve(const std::string &identifier) {
  return _context.resolve(identifier);
}

std::string
ReactionEditorDisplayContext::identifier(GiNaC::symbol symbol) const {
  std::string id;
  try { id = _context.identifier(symbol); }
  catch (iNA::Exception &err) { id = symbol.get_name(); }
  return id;
}

bool
ReactionEditorDisplayContext::hasConcentrationUnits(const GiNaC::symbol &symbol) {
  std::string id;
  try { id = _context.identifier(symbol); }
  catch (iNA::Exception &err) { id = symbol.get_name(); }
  return _context.hasConcentrationUnits() && _context.hasSpecies(id);
}



/* ********************************************************************************************* *
 * Implementation of ReactionEditor
 * ********************************************************************************************* */
ReactionEditor::ReactionEditor(iNA::Ast::Model &model, iNA::Ast::Reaction *reaction, QWidget *parent)
  : QWizard(parent), _model(model), _current_reaction(reaction),
    _context(&model, 0 == reaction ? (iNA::Ast::Scope *)&model : (iNA::Ast::Scope *)reaction->getKineticLaw())
{
  if (0 == reaction)
    setWindowTitle(tr("Create new reaction"));
  else
    setWindowTitle(tr("Edit reaction"));

  // Assemble wizard pages:
  addPage(new ReactionEditorPage(reaction, this));
  addPage(new ReactionEditorSummaryPage(this));
}


iNA::Ast::Model &
ReactionEditor::model()
{
  return _model;
}

ReactionEditorContext &
ReactionEditor::context() {
  return _context;
}

const StoichiometryList &
ReactionEditor::reactants() const {
  return _reactants;
}

StoichiometryList &
ReactionEditor::reactants() {
  return _reactants;
}

void
ReactionEditor::setReactants(const StoichiometryList &list) {
  _reactants = list;
}

const StoichiometryList &
ReactionEditor::products() const {
  return _products;
}

StoichiometryList &
ReactionEditor::products() {
  return _products;
}

void
ReactionEditor::setProducts(const StoichiometryList &list) {
  _products = list;
}

const GiNaC::ex &
ReactionEditor::kineticLaw() const {
  return _kineticLaw;
}

GiNaC::ex &
ReactionEditor::kineticLaw() {
  return _kineticLaw;
}

void
ReactionEditor::setKinteticLaw(const GiNaC::ex &expr) {
  _kineticLaw = expr;
}

bool
ReactionEditor::isReversible() const {
  return _is_reversible;
}

void
ReactionEditor::setReversible(bool rev) {
  _is_reversible = rev;
}

const QString &
ReactionEditor::reactionName() const {
  return _reactionName;
}

void
ReactionEditor::setReactionName(const QString &name) {
  _reactionName = name;
}

iNA::Ast::Reaction *
ReactionEditor::reaction() {
  return _current_reaction;
}



/* ********************************************************************************************* *
 * Implementation of ReactionEditorPage
 * ********************************************************************************************* */
ReactionEditorPage::ReactionEditorPage(iNA::Ast::Reaction *reaction, ReactionEditor *editor)
  : QWizardPage(editor), _model(editor->model()), _current_reaction(reaction),
    _editor(editor)
{
  setTitle(tr("Specify chemical equation and propensity"));

  // The name field
  _name = new QLineEdit("reaction");
  _name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  // Initialize name line edit if a reaction is given:
  if ((0 != _current_reaction) && (_current_reaction->hasName())) {
    _name->setText(_current_reaction->getName().c_str());
  }

  // The reactants stoichiometry
  _equation = new ExpressionEditor(_model);
  _equation->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  /*QCompleter *completer = new QCompleter(ScopeItemModel::collectIdentifiers(_model, ScopeItemModel::SELECT_ALL));
  _equation->setCompleter(completer); */
  if ((0 != _current_reaction)) {
    _equation->setText(_serializeReactionEquation());
  }

  // Get default color and define error color for equation editor:
  _default_background = _equation->palette().color(QPalette::Base);
  _error_background = QColor(Qt::red).lighter();

  // The kinetic law expression
  _kinetic_law_type = new QComboBox();
  _kinetic_law_type->addItem("Mass action", (unsigned) MASSACTION_SINGLE);
  _kinetic_law_type->addItem("Multi-compartment", (unsigned) MASSACTION_MULTI);
  _kinetic_law_type->addItem("User defined", (unsigned) USER_DEFINED);
  // Select mass action no reaction is given, user defined otherwise
  if (0 == _current_reaction) { _kinetic_law_type->setCurrentIndex(0); }
  else { _kinetic_law_type->setCurrentIndex(2); }

  _kineticLaw = new QStackedWidget();
  _kineticLawFormula = new QLabel();
  _kineticLaw->addWidget(_kineticLawFormula);
  _kineticLawEditor = new ExpressionEditor(_model);
  _kineticLawEditor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  _kineticLawEditor->setEnabled(true);
  _kineticLaw->addWidget(_kineticLawEditor);
  if (0 == _current_reaction) { _kineticLaw->setCurrentIndex(0); }
  else { _kineticLaw->setCurrentIndex(1); }


  // Initialize kinetic expression law editor if a reaction is given:
  if (0 != _current_reaction) {
    _kineticLawEditor->setText(_serializePropensity());
  }

  // Layout
  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("Name"), _name);
  layout->addRow(tr("Chemical equation"), _equation);
  layout->addRow(tr("Type"), _kinetic_law_type);
  layout->addRow(tr("Propensity"), _kineticLaw);
  setLayout(layout);

  // Delay timer
  _delayTimer = new QTimer(); _delayTimer->setSingleShot(true); _delayTimer->setInterval(500);

  // Connect all signals
  QObject::connect(_kineticLawEditor, SIGNAL(textChanged(QString)), this, SLOT(_onKineticLawExpressionChanged()));
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

  is_valid = _parseReactionEquation(_equation->text(), reactants, products, is_reversible);
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
  if (! _parseReactionEquation(_equation->text(), reactants, products, is_reversible)) {
    QPalette equation_palette = _equation->palette();
    equation_palette.setColor(QPalette::Base, _error_background);
    _equation->setPalette(equation_palette);
    return;
  }

  // If equation was parsed correctly -> set default background color:
  QPalette equation_palette = _equation->palette();
  equation_palette.setColor(QPalette::Base, _default_background);
  _equation->setPalette(equation_palette);

  // Clear context and define new species
  _editor->context().reset();
  _defineUnknownSpecies(reactants, products);

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


void
ReactionEditorPage::_onKineticLawExpressionChanged()
{
  // Try to parse the expression:
  if (USER_DEFINED == kineticLawType())
  {
    ReactionEditorContext ctx(&_model, &_model);
    try {
      // Try to parse the expression
      iNA::Parser::Expr::parseExpression(_kineticLawEditor->text().toStdString(), ctx);
      // On success, reset formula background color
      QPalette palette = _kineticLawEditor->palette();
      palette.setColor(QPalette::Base, _default_background);
      _kineticLawEditor->setPalette(palette);
    } catch (iNA::Exception &err) {
      // On on, set formula background color to red
      QPalette palette = _kineticLawEditor->palette();
      palette.setColor(QPalette::Base, _error_background);
      _kineticLawEditor->setPalette(palette);
    }
  }
}



QString
ReactionEditorPage::_serializeReactionEquation()
{
  QStringList reactants, products;

  // First serialize reactants:
  for (iNA::Ast::Reaction::iterator reac=_current_reaction->reactantsBegin();
       reac!=_current_reaction->reactantsEnd(); reac++)
  {
    std::stringstream buffer;
    iNA::Parser::Expr::serializeExpression(reac->second, buffer, &_model);
    reactants.append(
          reac->second!=1 ? QString("%1*%2").arg(buffer.str().c_str()).arg(reac->first->getIdentifier().c_str())
                          : QString("%1").arg(reac->first->getIdentifier().c_str()));
  }

  // then, serialize products:
  for (iNA::Ast::Reaction::iterator prod=_current_reaction->productsBegin();
       prod!=_current_reaction->productsEnd(); prod++)
  {
    std::stringstream buffer;
    iNA::Parser::Expr::serializeExpression(prod->second, buffer, &_model);
    products.append(
          prod->second!=1 ? QString("%1*%2").arg(buffer.str().c_str()).arg(prod->first->getIdentifier().c_str())
                          : QString("%1").arg(prod->first->getIdentifier().c_str()));

  }

  // finally, assemble equation:
  if (_current_reaction->isReversible()) {
    return QString("%1 = %2").arg(reactants.join(" + ")).arg(products.join(" + "));
  }
  return QString("%1 -> %2").arg(reactants.join(" + ")).arg(products.join(" + "));
}


QString
ReactionEditorPage::_serializePropensity()
{
  iNA::Ast::KineticLaw *kinlaw = _current_reaction->getKineticLaw();
  std::stringstream buffer;
  iNA::Parser::Expr::serializeExpression(kinlaw->getRateLaw(), buffer, kinlaw);
  return buffer.str().c_str();
}



bool
ReactionEditorPage::_parseReactionEquation(
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


std::set<std::string>
ReactionEditorPage::_collectCompartments(QList<QPair<int, QString> > &reactants,
                                         QList<QPair<int, QString> > &products)
{
  std::set<std::string> compartments;

  // The compartment ID, new species are created in.
  std::string def_compartment = _editor->context().compartmentIdentifier();

  // Handle list of reactants:
  for (QList< QPair<int, QString> >::iterator item=reactants.begin(); item!=reactants.end(); item++)
  {
    // If species is not defined yet, use default compartment
    if (! _model.hasSpecies(item->second.toStdString())) {
      compartments.insert(def_compartment);
    } else {
      // otherwise use compartment of species
      compartments.insert(
            _model.getSpecies(item->second.toStdString())->getCompartment()->getIdentifier());
    }
  }
  // Handle list of products
  for (QList< QPair<int, QString> >::iterator item=products.begin(); item!=products.end(); item++)
  {
    // If species is not defined yet, use default compartment
    if (! _model.hasSpecies(item->second.toStdString())) {
      compartments.insert(def_compartment);
    } else {
      // otherwise use compartment of species
      compartments.insert(
            _model.getSpecies(item->second.toStdString())->getCompartment()->getIdentifier());
    }
  }

  return compartments;
}

std::map<QString,int>
ReactionEditorPage::_collectStoichiometries(QList<QPair<int, QString> > &reactants)
{
  // Collect reactants and assemble stoichiometries
  std::map<QString,int> stoichiometries;
  for (QList< QPair<int, QString> >::iterator item=reactants.begin(); item != reactants.end(); item++)
  {
    std::map< QString, int >::iterator it= stoichiometries.find(item->second) ;
    if( it!=stoichiometries.end()) {
      it->second+=item->first;
    } else {
      stoichiometries.insert(std::pair<QString,int>(item->second,item->first));
    }
  }

  return stoichiometries;
}


MathItem *
ReactionEditorPage::_renderKineticLaw(bool is_reversible, StoichiometryList &reactants,
                                      StoichiometryList &products)
{
  // Assemble kinetic law
  GiNaC::ex kinLaw = _createKineticLaw(reactants, products, is_reversible);
  iNA::Ast::Scope *scope = &_model;
  if (0 != _current_reaction) { scope = _current_reaction->getKineticLaw(); }
  return Ginac2Formula::toFormula(kinLaw, *scope, true);
}


void
ReactionEditorPage::_defineUnknownSpecies(const StoichiometryList &reactants,
                                          const StoichiometryList &products)
{
  // Process reactants...
  for (StoichiometryList::const_iterator item = reactants.begin(); item!=reactants.end(); item++) {
    // If identifier names a species -> skip
    if (_editor->model().hasSpecies(item->second.toStdString())) { continue; }
    if (_editor->context().hasSpecies(item->second.toStdString())) { continue; }

    // if identifier names any other definition -> error.
    if (_model.hasDefinition(item->second.toStdString())) {
      iNA::SymbolError err;
      err << "Can not define identifier " << item->second.toStdString() << " as a species, "
          << " identifier already used by an other definition.";
      throw err;
    }
    // (pre) define species in context
    _editor->context().getOrCreateSpecies(item->second.toStdString());
  }

  // Process products...
  for (StoichiometryList::const_iterator item = products.begin(); item!=products.end(); item++) {
    // If identifier names a species -> skip
    if (_editor->model().hasSpecies(item->second.toStdString())) { continue; }
    if (_editor->context().hasSpecies(item->second.toStdString())) { continue; }

    // if identifier names any other definition -> error.
    if (_model.hasDefinition(item->second.toStdString())) {
      iNA::SymbolError err;
      err << "Can not define identifier " << item->second.toStdString() << " as a species, "
          << " identifier already used by an other definition.";
      throw err;
    }
    // (pre) define species in context
    _editor->context().getOrCreateSpecies(item->second.toStdString());
  }
}


GiNaC::ex
ReactionEditorPage::_createKineticLaw(const StoichiometryList &reactants,
                                      const StoichiometryList &products,
                                      bool is_reversible)
{
  switch (kineticLawType()) {
  case MASSACTION_SINGLE:
  case MASSACTION_MULTI:
    return _createMAKineticLaw(reactants, products, is_reversible);
  case USER_DEFINED: break;
  }
  return _parseAndCreateKineticLaw();
}


GiNaC::ex
ReactionEditorPage::_createMAKineticLaw(const StoichiometryList &reactants,
                                        const StoichiometryList &products,
                                        bool is_reversible)
{
  GiNaC::symbol k_fwd, k_rev;

  // Define local parameters:
  if (! is_reversible) {
    k_fwd = _editor->context().getOrCreateLocalParameter("k");
  } else {
    k_fwd = _editor->context().getOrCreateLocalParameter("k_fwd");
    k_rev = _editor->context().getOrCreateLocalParameter("k_rev");
  }

  GiNaC::ex factor_fwd = 0;
  GiNaC::ex factor_rev = 0;

  factor_fwd = k_fwd;
  if (is_reversible) { factor_rev = k_rev; }

  if (MASSACTION_SINGLE == kineticLawType()) {
    GiNaC::symbol compartment;
    // Obtain compartment of reaction:
    for (StoichiometryList::const_iterator reac=reactants.begin(); reac != reactants.end(); reac++) {
      if (_model.hasSpecies(reac->second.toStdString())) {
        compartment = _model.getSpecies(reac->second.toStdString())->getCompartment()->getSymbol();
      } else {
        compartment = _editor->context().compartmentSymbol();
      }
      break;
    }
    for (StoichiometryList::const_iterator prod=products.begin(); prod != products.end(); prod++) {
      if (_model.hasSpecies(prod->second.toStdString())) {
        compartment = _model.getSpecies(prod->second.toStdString())->getCompartment()->getSymbol();
      } else {
        compartment = _editor->context().compartmentSymbol();
      }
      break;
    }

    factor_fwd *= compartment;
    factor_rev *= compartment;
  }

  // Iterate over reactants:
  for (StoichiometryList::const_iterator reac=reactants.begin(); reac!=reactants.end(); reac++) {
    factor_fwd *= _createMAFactor(reac->second, reac->first);
  }

  // Iterate over products if reaction is reversible:
  if (is_reversible) {
    // Iterate over reactants:
    for (StoichiometryList::const_iterator reac=reactants.begin(); reac!=reactants.end(); reac++) {
      factor_fwd *= _createMAFactor(reac->second, reac->first);
    }
  }

  // Assemble and assign kinetic law:
  return factor_fwd - factor_rev;
}


GiNaC::ex
ReactionEditorPage::_createMAFactor(const QString &species, GiNaC::ex stoichiometry)
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
ReactionEditorPage::_createMASingleFactor(const QString &species, int stoichiometry)
{
  if (0 >= stoichiometry) { return 1; }

  // Get species symbol
  GiNaC::ex species_expr = _editor->context().resolve(species.toStdString());
  // Get compartment symbol
  GiNaC::symbol compartment = _editor->context().compartmentSymbol();
  if (_model.hasSpecies(species.toStdString())) {
    compartment = _model.getSpecies(species.toStdString())->getCompartment()->getSymbol();
  }
  // If species are defined in substance units:
  if (_model.speciesHaveSubstanceUnits()) { species_expr /= compartment; }

  GiNaC::ex factor=species_expr;
  for (int i=1; i<stoichiometry; i++) {
    factor *= (species_expr- i/compartment);
  }

  return factor;
}


GiNaC::ex
ReactionEditorPage::_createMAMultiFactor(const QString &species, int stoichiometry)
{
  if (0 >= stoichiometry) { return 1; }

  // Get species symbol
  GiNaC::ex species_expr = _editor->context().resolve(species.toStdString());
  // get species compartment
  GiNaC::symbol compartment = _editor->context().compartmentSymbol();
  if (_model.hasSpecies(species.toStdString())) {
    compartment = _model.getSpecies(species.toStdString())->getSymbol();
  }

  // If species are defined in substance units:
  if (_model.speciesHaveSubstanceUnits()) {
    species_expr /= compartment;
  }

  return GiNaC::pow(species_expr, stoichiometry);
}


GiNaC::ex
ReactionEditorPage::_parseAndCreateKineticLaw()
{
  // Parse kinetic law expression:
  return iNA::Parser::Expr::parseExpression( _kineticLawEditor->text().toStdString(), _editor->context());
}


void
ReactionEditorPage::_updateCurrentReaction(
    const QString &name, QList<QPair<int, QString> > &reactants,
    QList<QPair<int, QString> > &products, bool is_reversible, iNA::Ast::Scope *scope)
{
  // Update reaction name:
  _current_reaction->setName(name.toStdString());

  // Update reversible reaction
  _current_reaction->setReversible(is_reversible);

  // Clear reactants, products of the reaction
  _current_reaction->clearReactants();
  _current_reaction->clearProducts();

  // Create reactants:
  for (QList< QPair<int, QString> >::iterator item=reactants.begin(); item!=reactants.end(); item++) {
    iNA::Ast::Definition *def = scope->getDefinition(item->second.toStdString());
    if (! iNA::Ast::Node::isSpecies(def)) {
      iNA::InternalError err;
      err << "Identifier " << item->second.toStdString()
          << " in reactants list does not name a species.";
      throw err;
    }

    _current_reaction->addReactantStoichiometry(static_cast<iNA::Ast::Species *>(def), item->first);
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

    _current_reaction->addProductStoichiometry(static_cast<iNA::Ast::Species *>(def), item->first);
  }
}



bool
ReactionEditorPage::validatePage()
{
  // Check reaction equation:
  QList< QPair<int, QString> > reactants, products;
  bool is_reversible = false;
  // Try to parse the expression
  if (! _parseReactionEquation(_equation->text(), reactants, products, is_reversible)) {
    return false;
  }

  // Clear context, all pre-defined species etc are removed.
  _editor->context().reset();
  // Update reaction editor context
  _defineUnknownSpecies(reactants, products);

  // Check kinetic law (if defined by user):
  if (USER_DEFINED == kineticLawType()) {
    try {
      // Try to parse the expression
      iNA::Parser::Expr::parseExpression(_kineticLawEditor->text().toStdString(), _editor->context());
      // On success, reset formula background color
      QPalette palette = _kineticLawEditor->palette();
      palette.setColor(QPalette::Base, _default_background);
      _kineticLawEditor->setPalette(palette);
    } catch (iNA::Exception &err) {
      // On on, set formula background color to red
      QPalette palette = _kineticLawEditor->palette();
      palette.setColor(QPalette::Base, _error_background);
      _kineticLawEditor->setPalette(palette);
      // and send a message to the logger
      iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::INFO);
      message << "Invalid kinetic law expression: " << err.what();
      iNA::Utils::Logger::get().log(message);
      return false;
    }
  }

  // Create kinetic law for that reaction and store in editor
  try {
    _editor->setKinteticLaw(_createKineticLaw(reactants, products, is_reversible));
  } catch (iNA::Exception &err) {
    QMessageBox::critical(
          0, tr("Error in kinetic law"),
          tr("Error in assembling kinetic law expression: %1").arg(err.what()));
    return false;
  }

  // Update reactants and product stoichiometry
  _editor->setReactants(reactants);
  _editor->setProducts(products);
  // Update reverisble flag in editor.
  _editor->setReversible(is_reversible);

  // Strip whitespaces from string (also replace multiple WS by one)
  _editor->setReactionName(_name->text().simplified());

  // done...
  return true;
}



/* ********************************************************************************************* *
 * Implementation of ReactionEditorSummaryPage
 * ********************************************************************************************* */
ReactionEditorSummaryPage::ReactionEditorSummaryPage(ReactionEditor *wizard)
  : QWizardPage(wizard), _model(wizard->model()), _editor(wizard)
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
  _created_compartments = new QLabel();
  species_layout->addRow(tr("Created compartments"), _created_compartments);
  _created_parameters = new QLabel();
  species_layout->addRow(tr("Created parameters"), _created_parameters);

  // Assemble layout
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(prev_box);
  layout->addLayout(species_layout);
  setLayout(layout);
}


void
ReactionEditorSummaryPage::initializePage()
{
  // Render equation
  MathItem *reac = _renderReactionPreview(_editor->reactants(), _editor->products(),
                                          _editor->kineticLaw(), _editor->isReversible());
  _reaction_preview->setPixmap(reac->renderItem());
  delete reac;

  // Assemble list of created species:
  QStringList created_species;
  std::map<std::string, GiNaC::symbol>::const_iterator item =
      _editor->context().undefinedSpecies().begin();
  for (; item!=_editor->context().undefinedSpecies().end(); item++) {
    created_species.append(item->first.c_str());
  }
  if (0 == created_species.size()) {
    _created_species->setText(tr("<none>"));
  } else {
    _created_species->setText(created_species.join(", "));
  }

  // Assemble list of created compartments:
  if (_editor->context().compartmentIsUndefined()) {
    _created_compartments->setText(_editor->context().compartmentIdentifier().c_str());
  } else {
    _created_compartments->setText(tr("<none>"));
  }

  // Assemble list of created parameters:
  QStringList created_parameters;
  item = _editor->context().undefinedParameters().begin();
  for (; item!=_editor->context().undefinedParameters().end(); item++) {
    created_parameters.append(item->first.c_str());
  }
  if (0 == created_parameters.size()) {
    _created_parameters->setText(tr("<none>"));
  } else {
    _created_parameters->setText(created_parameters.join(", "));
  }
}


MathItem *
ReactionEditorSummaryPage::_renderReactionPreview(
    const StoichiometryList &reactants,  const StoichiometryList &products,
    const GiNaC::ex &kineticLaw, bool is_reversible)
{
  MathFormula *formula = new MathFormula();
  QList<QPair<int, QString> >::const_iterator item = reactants.begin();
  for(int idx=0; item != reactants.end(); item++, idx++) {
    int sto        = item->first;
    std::string id = item->second.toStdString();
    if (0 != idx) {
      formula->appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));
      formula->appendItem(new MathText(" + "));
      formula->appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));
    }
    if (1 != sto) {
      formula->appendItem(new MathText(QString("%1").arg(sto)));
      formula->appendItem(new MathSpace(MathSpace::THIN_SPACE));
    }
    if (_model.hasSpecies(id)) {
      formula->appendItem(TinyTex::parseQuoted(_model.getSpecies(id)->getLabel()));
    } else {
      formula->appendItem(new MathText(id.c_str()));
    }
  }

  formula->appendItem(new MathSpace(MathSpace::THICK_SPACE));
  if (is_reversible) { formula->appendItem(new MathText(QChar(0x21CC))); }
  else { formula->appendItem(new MathText(QChar(0x2192))); }
  formula->appendItem(new MathSpace(MathSpace::THICK_SPACE));

  item = products.begin();
  for(int idx=0; item != products.end(); item++, idx++) {
    int sto        = item->first;
    std::string id = item->second.toStdString();
    if (0 != idx) {
      formula->appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));
      formula->appendItem(new MathText(" + "));
      formula->appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));
    }
    if (1 != sto) {
      formula->appendItem(new MathText(QString("%1").arg(sto)));
      formula->appendItem(new MathSpace(MathSpace::THIN_SPACE));
    }
    if (_model.hasSpecies(id)) {
      formula->appendItem(TinyTex::parseQuoted(_model.getSpecies(id)->getLabel()));
    } else {
      formula->appendItem(new MathText(id.c_str()));
    }
  }

  formula->appendItem(new MathSpace(MathSpace::QUAD_SPACE));
  formula->appendItem(new MathText(":"));
  formula->appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));
  ReactionEditorDisplayContext context(_editor->context());
  formula->appendItem(Ginac2Formula::toFormula(kineticLaw, context));
  return formula;
}
