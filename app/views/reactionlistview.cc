#include "reactionlistview.hh"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include "../models/application.hh"
#include "../doctree/documenttree.hh"
#include "reactioneditor.hh"
#include <utils/logger.hh>
#include <ast/identifier.hh>


ReactionListView::ReactionListView(ReactionsItem *reactions, QWidget *parent)
  : QWidget(parent), _reactions(reactions->reactionList()), _reactions_item(reactions)
{
  // Basic layout
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  setBackgroundRole(QPalette::Window);

  // label
  QLabel *label = new QLabel(tr("Reactions"));
  label->setFont(Application::getApp()->getH1Font());
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  label->setAlignment(Qt::AlignRight);

  // "new reaction" button
  _newReacButton = new QPushButton("+");
  _newReacButton->setToolTip(tr("Add reaction"));
  _newReacButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // rem reaction button
  _remReacButton = new QPushButton("-");
  _remReacButton->setToolTip(tr("Delete reaction"));
  _remReacButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  _remReacButton->setEnabled(false);

  // Assemble list view:
  _reactionList = new QTableView();
  _reactionList->setModel(_reactions);
  _reactionList->setSelectionMode(QAbstractItemView::SingleSelection);
  _reactionList->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

  QHBoxLayout *header_layout = new QHBoxLayout();
  header_layout->addWidget(_newReacButton, 0, Qt::AlignLeft);
  header_layout->addWidget(_remReacButton, 0, Qt::AlignLeft);
  header_layout->addWidget(label, 1, Qt::AlignRight);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(header_layout);
  layout->addWidget(_reactionList,1);
  setLayout(layout);

  QObject::connect(reactions, SIGNAL(destroyed()), this, SLOT(deleteLater()));
  QObject::connect(_newReacButton, SIGNAL(clicked()), this, SLOT(onNewReaction()));
  QObject::connect(_remReacButton, SIGNAL(clicked()), this, SLOT(onRemReaction()));
  QObject::connect(
        _reactionList->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(onSelectionChanged(QItemSelection,QItemSelection)));
  QObject::connect(_reactionList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onReactionEditing(QModelIndex)));
}


void
ReactionListView::onNewReaction()
{
  // Show reaction editor wizard:
  ReactionEditor editor(_reactions->getModel());
  if (QDialog::Rejected == editor.exec()) { return; }

  iNA::Ast::Compartment *compartment = 0;
  GiNaC::exmap subst_table;
  iNA::Ast::Model &model = _reactions->getModel();

  // If a compartment needs to be created
  if (editor.context().compartmentIsUndefined()) {
    // Get an new unique ID for the compartment
    compartment = new iNA::Ast::Compartment(editor.context().compartmentIdentifier(), 1,
                                            iNA::Ast::Compartment::VOLUME);
    subst_table[editor.context().compartmentSymbol()] = compartment->getSymbol();
    model.addDefinition(compartment);
  } else {
    // Resolve compartment to be used to define new species in
    if (! model.hasCompartment(editor.context().compartmentIdentifier())) {
      QMessageBox::critical(0, tr("Can not create reaction"),
                            tr("Internal error: Can not resolve compartment '%1' in model.").arg(
                              editor.context().compartmentIdentifier().c_str()));
      return;
    }
    compartment = model.getCompartment(editor.context().compartmentIdentifier());
  }

  // Define undefined species
  std::map<std::string, GiNaC::symbol>::const_iterator spec = editor.context().undefinedSpecies().begin();
  for (; spec != editor.context().undefinedSpecies().end(); spec++) {
    iNA::Ast::Species *species = new iNA::Ast::Species(spec->first, 0, compartment, "", false);
    subst_table[spec->second] = species->getSymbol();
    model.addDefinition(species);
  }

  // Assemble kinetic law
  iNA::Ast::KineticLaw *law = new iNA::Ast::KineticLaw(0);
  // define local parameters
  std::map<std::string, GiNaC::symbol>::const_iterator para = editor.context().undefinedParameters().begin();
  for (; para != editor.context().undefinedParameters().end(); para++) {
    iNA::Ast::Parameter *parameter = new iNA::Ast::Parameter(
          para->first, 1, iNA::Ast::Unit::dimensionless(), true);
    subst_table[para->second] = parameter->getSymbol();
    law->addDefinition(parameter);
  }
  // Set rate law (substituted)
  law->setRateLaw(editor.kineticLaw().subs(subst_table));

  // Obtain a new unique and valid ID for reaction from reaction name
  QString id_base = editor.reactionName(); id_base.replace(QRegExp("[^a-zA-Z0-9_]"), "_");
  if (! iNA::Ast::isValidId(id_base.toStdString())) { id_base = "reaction"; }
  std::string reac_id = model.getNewIdentifier(id_base.toStdString());
  // create reaction
  iNA::Ast::Reaction *reaction = new iNA::Ast::Reaction(
        reac_id, editor.reactionName().toStdString(), law, editor.isReversible());

  // Populate reactants
  StoichiometryList::const_iterator reactant = editor.reactants().begin();
  for (; reactant != editor.reactants().end(); reactant++) {
    iNA::Ast::Species *species = model.getSpecies(reactant->second.toStdString());
    reaction->addReactantStoichiometry(species, reactant->first);
  }
  // Populate products
  StoichiometryList::const_iterator product = editor.products().begin();
  for (; product != editor.products().end(); product++) {
    iNA::Ast::Species *species = model.getSpecies(product->second.toStdString());
    reaction->addProductStoichiometry(species, product->first);
  }

  // Done. Add reaction to model:
  model.addDefinition(reaction);

  /// @todo There is no need to reset the complete tree as the reaction just gets inserted.
  ///       Therefore only a update of the ReactionListView needs to be performed. And the index
  ///       of the inserted item in the DocTreeModel is know.

  // Update document tree
  Application::getApp()->resetSelectedItem();
  Application::getApp()->docTree()->resetCompleteTree();
}


void
ReactionListView::onRemReaction()
{
  // Get selected reaction:
  QModelIndexList selected_items = _reactionList->selectionModel()->selectedIndexes();
  // If more than one element is selected -> quit
  if (1 != selected_items.size()) { return; }
  // Get selected model index
  QModelIndex idx = selected_items.at(0);

  // Get ReactionItem for that reaction:
  ReactionItem *reac_item = dynamic_cast<ReactionItem *>(_reactions_item->getTreeChild(idx.row()));
  if (0 == reac_item) {
    iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::WARN);
    message << "Can not remove selected item: it is not a reaction!";
    iNA::Utils::Logger::get().log(message);
    return;
  }

  // Last chance to quit
  if (QMessageBox::Yes != QMessageBox::question(
        0, tr("Remove reaction."),
        tr("Are you sure to delete the reaction, this can not be undone."),
        QMessageBox::Yes, QMessageBox::No)) { return; }

  // Remove selected reaction
  Application::getApp()->docTree()->removeReaction(reac_item);
  _reactions->updateTable();
}


void
ReactionListView::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deseleced)
{
  QModelIndexList indices = selected.indexes();
  if (1 != indices.size()) { _remReacButton->setEnabled(false); return; }
  QModelIndex index = indices.at(0);
  if (0 > index.column()) { _remReacButton->setEnabled(false); return; }
  _remReacButton->setEnabled(true);
}


void
ReactionListView::onReactionEditing(const QModelIndex &index)
{
  // Get selected reaction:
  if (index.row() >= (int)_reactions->getModel().numReactions()) { return; }
  if (index.row() < 0) { return; }
  iNA::Ast::Reaction *reaction = _reactions->getModel().getReaction(index.row());

  // Show reaction editor wizard:
  ReactionEditor editor(_reactions->getModel(), reaction);
  if (QDialog::Rejected == editor.exec()) { return; }

  iNA::Ast::Compartment *compartment = 0;
  GiNaC::exmap subst_table;
  iNA::Ast::Model &model = _reactions->getModel();

  // If a compartment needs to be created
  if (editor.context().compartmentIsUndefined()) {
    // Get an new unique ID for the compartment
    compartment = new iNA::Ast::Compartment(editor.context().compartmentIdentifier(), 1,
                                            iNA::Ast::Compartment::VOLUME);
    subst_table[editor.context().compartmentSymbol()] = compartment->getSymbol();
    model.addDefinition(compartment);
  } else {
    // Resolve compartment to be used to define new species in
    if (! model.hasCompartment(editor.context().compartmentIdentifier())) {
      QMessageBox::critical(0, tr("Can not create reaction"),
                            tr("Internal error: Can not resolve compartment '%1' in model.").arg(
                              editor.context().compartmentIdentifier().c_str()));
      return;
    }
    compartment = model.getCompartment(editor.context().compartmentIdentifier());
  }

  // Define undefined species
  std::map<std::string, GiNaC::symbol>::const_iterator spec = editor.context().undefinedSpecies().begin();
  for (; spec != editor.context().undefinedSpecies().end(); spec++) {
    iNA::Ast::Species *species = new iNA::Ast::Species(spec->first, 0, compartment, "", false);
    subst_table[spec->second] = species->getSymbol();
    model.addDefinition(species);
  }

  // Update reaction name
  reaction->setName(editor.reactionName().toStdString());
  // Update reactant stoichiometry
  reaction->clearReactants();
  StoichiometryList::const_iterator reactant = editor.reactants().begin();
  for (; reactant != editor.reactants().end(); reactant++) {
    iNA::Ast::Species *species = model.getSpecies(reactant->second.toStdString());
    reaction->addReactantStoichiometry(species, reactant->first);
  }
  // Update product stoichiometry
  reaction->clearProducts();
  StoichiometryList::const_iterator product = editor.products().begin();
  for (; product != editor.products().end(); product++) {
    iNA::Ast::Species *species = model.getSpecies(product->second.toStdString());
    reaction->addProductStoichiometry(species, product->first);
  }
  // Clear modifiers
  reaction->clearModifier();

  // Assemble kinetic law
  iNA::Ast::KineticLaw *law = reaction->getKineticLaw();
  // define local parameters
  std::map<std::string, GiNaC::symbol>::const_iterator para = editor.context().undefinedParameters().begin();
  for (; para != editor.context().undefinedParameters().end(); para++) {
    iNA::Ast::Parameter *parameter = new iNA::Ast::Parameter(
          para->first, 1, iNA::Ast::Unit::dimensionless(), true);
    subst_table[para->second] = parameter->getSymbol();
    law->addDefinition(parameter);
  }
  // Set rate law (substituted)
  law->setRateLaw(editor.kineticLaw().subs(subst_table));

  /// @todo There is no need to reset the complete tree as the reaction just gets updated.
  ///       Therefore only a update of the ReactionListView needs to be performed. Also the
  ///       label of the reaction in the document tree needs to be updated.

  // Update document tree
  Application::getApp()->resetSelectedItem();
  Application::getApp()->docTree()->resetCompleteTree();
}
