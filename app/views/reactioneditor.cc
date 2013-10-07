#include "reactioneditor.hh"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QCompleter>
#include <QGroupBox>

#include "reactionequationrenderer.hh"
#include "../tinytex/tinytex.hh"
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

void
ReactionEditorContext::reset() {
  _param_ids.clear();
  _param_symbols.clear();
  _species_ids.clear();
  _species_symbols.clear();
}


/* ********************************************************************************************* *
 * Implementation of ReactionEditor
 * ********************************************************************************************* */
ReactionEditor::ReactionEditor(iNA::Ast::Model &model, iNA::Ast::Reaction *reaction, QWidget *parent)
  : QWizard(parent), _model(model), _current_reaction_scope(0), _current_reaction(reaction),
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

iNA::Ast::Reaction *
ReactionEditor::reaction()
{
  // If we edit an existing reaction -> return this reaction
  if (0 != _current_reaction) { return _current_reaction; }

  // Otherwise get reaction from reaction scope:
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
ReactionEditorPage::ReactionEditorPage(iNA::Ast::Reaction *reaction, ReactionEditor *editor)
  : QWizardPage(editor), _model(editor->model()), _current_reaction(reaction),
    _context(editor->context())
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


std::set<std::string>
ReactionEditorPage::_collectCompartments(QList<QPair<int, QString> > &reactants,
                                         QList<QPair<int, QString> > &products)
{
  std::set<std::string> compartments;

  // The compartment ID, new species are created in.
  std::string def_compartment = _context.compartmentIdentifier();

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
ReactionEditorPage::_renderKineticLaw(bool is_reversible, QList<QPair<int, QString> > &reactants,
                                      QList<QPair<int, QString> > &products)
{

  // Collect reactants and assemble stoichiometries
  std::map<QString,int> reactantsStoi = _collectStoichiometries(reactants);

  std::set<std::string> compartments = _collectCompartments(reactants, products);

  MathFormula *formula = new MathFormula();

  // Handle reactants:
  if (is_reversible) {
    formula->appendItem(new MathSub(new MathText("k"), new MathText("fwd")));
  } else {
    formula->appendItem(new MathText("k"));
  }

  if (MASSACTION_SINGLE == kineticLawType()) {
    if(_model.speciesHaveSubstanceUnits())
    {
      int exponent = -1;
      for(std::map<QString,int>::iterator it=reactantsStoi.begin(); it!=reactantsStoi.end(); it++)
        exponent += it->second;
      switch(exponent)
      {
      case 0:
        break;
      case -1:
        formula->appendItem(new MathText(QChar(0x00B7)));
        formula->appendItem(_renderName(compartments.begin()->c_str()));
        break;
      default:
        formula->appendItem(new MathText(QChar(0x00B7)));
        formula->appendItem(new MathSup(_renderName(compartments.begin()->c_str()),
                                        new MathText(QString("-%1").arg(exponent))));
        break;
      }
    }
    else
    {
      formula->appendItem(new MathText(QChar(0x00B7)));
      formula->appendItem(_renderName(compartments.begin()->c_str()));
    }
  }


  for (std::map< QString, int>::iterator item=reactantsStoi.begin(); item != reactantsStoi.end(); item++)
  {
    formula->appendItem(new MathText(QChar(0x00B7)));
    formula->appendItem(_renderFactor(item->first, int(item->second)));
  }

  // If reaction is reversible, include reverse rate
  if (is_reversible) {

    // Collect products and assemble stoichiometries
    std::map<QString,int> productsStoi = _collectStoichiometries(products);

    formula->appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));
    formula->appendItem(new MathText("-"));
    formula->appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));

    formula->appendItem(new MathSub(new MathText("k"), new MathText("rev")));

    if (MASSACTION_SINGLE == kineticLawType()) {
      if(_model.speciesHaveSubstanceUnits())
      {
        int exponent = -1;
        for(std::map<QString,int>::iterator it=productsStoi.begin(); it!=productsStoi.end(); it++)
          exponent += it->second;
        switch(exponent)
        {
        case 0:
          break;
        case -1:
          formula->appendItem(new MathText(QChar(0x00B7)));
          formula->appendItem(_renderName(compartments.begin()->c_str()));
          break;
        default:
          formula->appendItem(new MathText(QChar(0x00B7)));
          formula->appendItem(new MathSup(_renderName(compartments.begin()->c_str()),
                                          new MathText(QString("-%1").arg(exponent))));
          break;
        }
      }
      else
      {
        formula->appendItem(new MathText(QChar(0x00B7)));
        formula->appendItem(_renderName(compartments.begin()->c_str()));
      }
    }

    for (std::map<QString,int>::iterator item=productsStoi.begin(); item != productsStoi.end(); item++)
    {
      formula->appendItem(new MathText(QChar(0x00B7)));
      formula->appendItem(_renderFactor(item->first, item->second));
    }

  }

  return formula;
}


MathItem *
ReactionEditorPage::_renderFactor(const QString &id, int exponent)
{

  MathFormula *name = new MathFormula();

  if(!_model.speciesHaveSubstanceUnits() || MASSACTION_MULTI == kineticLawType()) name->appendItem(new MathText("["));
  name->appendItem(_renderName(id));
  if(!_model.speciesHaveSubstanceUnits() || MASSACTION_MULTI == kineticLawType()) name->appendItem(new MathText("]"));

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
      }
      if(!_model.speciesHaveSubstanceUnits())
      {
        term->appendItem(new MathText(QChar(0x00B7)));
        term->appendItem(new MathSup(_renderCompartmentOf(id), new MathText("-1")));
      }
      else if(i==1)
      {
        term->appendItem(new MathText("1"));
      }
      factor->appendItem(new MathBlock(term, new MathText("("), new MathText(")")));
    }
  }

  delete name;
  return factor;
}


MathItem *
ReactionEditorPage::_renderCompartmentOf(const QString &id)
{
  // If the species is not define yet, use ID of first compartment
  // or the "dummy" ID as specified in context if there is no compartment defined at all.
  if (! _model.hasSpecies(id.toStdString())) {
    return _renderName(_context.compartmentIdentifier().c_str());
  }

  return _renderName(
        _model.getSpecies(id.toStdString())->getCompartment()->getIdentifier().c_str());
}


MathItem *
ReactionEditorPage::_renderName(const QString &id)
{
  // Assemble name of the factor
  if (_model.hasVariable(id.toStdString())) {
    iNA::Ast::VariableDefinition *var = _model.getVariable(id.toStdString());
    if (var->hasName()) {
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

    // Create a compartment if there is none.
    if (0 == _model.numCompartments()) {
      iNA::Ast::Compartment * new_compartment = new iNA::Ast::Compartment("compartment", 1, iNA::Ast::Compartment::VOLUME, true);
      /// @bug Modifies the model! The model should only be touched once the user finishes the
      /// wizard.
      _model.addDefinition(new_compartment);
    }

    // identifier not in use -> define species with this identifier.
    iNA::Ast::Species *new_species = new iNA::Ast::Species(
          item->second.toStdString(),_model.getCompartment(0), false);
    new_species->setValue(0);
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

    // Create a compartment if there is none.
    if (0 == _model.numCompartments()) {
      iNA::Ast::Compartment * new_compartment = new iNA::Ast::Compartment("compartment", 1, iNA::Ast::Compartment::VOLUME, true);
      /// @bug Modifies the model! The model should only be touched once the user finishes the
      /// wizard.
      _model.addDefinition(new_compartment);
    }

    // identifier not in use -> define species with this identifier.
    iNA::Ast::Species *new_species = new iNA::Ast::Species(
          item->second.toStdString(), _model.getCompartment(0), false);
    new_species->setValue(0);
    scope->addDefinition(new_species);
  }
}


iNA::Ast::Reaction *
ReactionEditorPage::_createReaction(const QString &name, QList<QPair<int, QString> > &reactants,
                                    QList<QPair<int, QString> > &products, bool is_reversible,
                                    iNA::Ast::Scope *scope)
{
  // Create unique identifier for reaction:
  // And remove all white spaces
  std::string identifier = name.simplified().replace(" ","").toStdString();
  if (! iNA::Ast::isValidId(identifier)) { identifier = "reaction"; }
  identifier = scope->getRootScope()->getNewIdentifier(identifier);

  // Create reaction and add to scope:
  iNA::Ast::Reaction *new_reaction = new iNA::Ast::Reaction(
              identifier, name.toStdString(), new iNA::Ast::KineticLaw(0), is_reversible);
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
  if (_model.speciesHaveSubstanceUnits()) {
    species_expr /= species->getCompartment()->getSymbol();
  }

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
  if (_model.speciesHaveSubstanceUnits()) {
    species_expr /= species->getCompartment()->getSymbol();
  }

  return GiNaC::pow(species_expr, stoichiometry);
}


void
ReactionEditorPage::_parseAndCreateKineticLaw(iNA::Ast::Reaction *reaction)
{
  // Parse kinetic law expression:
  iNA::Ast::KineticLaw *law = reaction->getKineticLaw();
  ReactionEditorContext ctx(&_model, law);
  GiNaC::ex rate_law = iNA::Parser::Expr::parseExpression(
        _kineticLawEditor->text().toStdString(), ctx);

  // get symbols of unresolved variables (parameters):
  GiNaC::exmap substitution;
  std::map<std::string, GiNaC::symbol> unresolved_symbols = ctx.undefinedParameters();
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


void
ReactionEditorPage::_updateCurrentReaction(
    const QString &name, QList<QPair<int, QString> > &reactants,
    QList<QPair<int, QString> > &products, bool is_reversible, iNA::Ast::Scope *scope)
{
  // Update reaction name:
  _current_reaction->setName(name.toStdString());

  // Update reversible reaction
  _current_reaction->setReversible(is_reversible);

  // Clear reactants, products and modifiers of the reaction
  _current_reaction->clearReactants();
  _current_reaction->clearProducts();
  _current_reaction->clearModifier();

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
  if (! _parseEquation(_equation->text(), reactants, products, is_reversible)) {
    return false;
  }

  // Check kinetic law (if defined by user):
  if (USER_DEFINED == kineticLawType()) {
    ReactionEditorContext ctx(&_model, &_model);
    try {
      // Try to parse the expression
      /// @bug Does not allow for a reference of new species and compartments
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
      // and send a message to the logger
      iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::INFO);
      message << "Invalid kinetic law expression: " << err.what();
      iNA::Utils::Logger::get().log(message);
      return false;
    }
  }

  // Construct new scope, holding all undefined variables:
  iNA::Ast::Scope *reaction_scope = new iNA::Ast::Scope(&_model);
  iNA::Ast::Reaction *new_reaction = 0;
  _defineUnknownSpecies(reactants, products, reaction_scope);


  if (0 == _current_reaction) {
    // Create reaction with empty kinetic law:
    new_reaction = _createReaction(_name->text(), reactants, products, is_reversible, reaction_scope);
    // Create kinetic law for that reaction
    _createKineticLaw(new_reaction);
  } else {
    // Update current reaction and its kinetic law:
    // Remove current reaction from _model:
    _model.remDefinition(_current_reaction); reaction_scope->addDefinition(_current_reaction);
    _updateCurrentReaction(_name->text(), reactants, products, is_reversible, reaction_scope);
    _createKineticLaw(_current_reaction);
    reaction_scope->remDefinition(_current_reaction); _model.addDefinition(_current_reaction);
  }

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
  ReactionEditor *editor = static_cast<ReactionEditor *>(wizard());

  // Render equation
  //MathContext ctx; ctx.setFontSize(ctx.fontSize()*0.66);
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

  // Assemble list of created compartments:
  QStringList created_compartments;
  for (iNA::Ast::Scope::iterator item=scope->begin(); item!=scope->end(); item++) {
    if (! iNA::Ast::Node::isCompartment(*item)) { continue; }
    created_compartments.append((*item)->getIdentifier().c_str());
  }
  if (0 == created_compartments.size()) {
    _created_compartments->setText(tr("<none>"));
  } else {
    _created_compartments->setText(created_species.join(", "));
  }

  // Assemble list of created parameters:
  QStringList created_parameters;
  for (iNA::Ast::Scope::iterator item=scope->begin(); item!=scope->end(); item++) {
    if (! iNA::Ast::Node::isParameter(*item)) { continue; }
    created_parameters.append((*item)->getIdentifier().c_str());
  }
  if (0 == created_parameters.size()) {
    _created_parameters->setText(tr("<none>"));
  } else {
    _created_parameters->setText(created_species.join(", "));
  }
}
