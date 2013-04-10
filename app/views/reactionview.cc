#include "reactionview.hh"
#include "../models/application.hh"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QTableView>
#include <QHeaderView>
#include <QMessageBox>

#include "reactioneditor.hh"
#include "../doctree/modelitem.hh"
#include "../doctree/documenttree.hh"
#include "reactionequationrenderer.hh"
#include "../models/expressiondelegate.hh"
#include <utils/logger.hh>
#include <trafo/makeparamglobal.hh>



ReactionView::ReactionView(ReactionItem *reaction, QWidget *parent) :
    QWidget(parent), _reaction(reaction)
{
  // Basic layout
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  setBackgroundRole(QPalette::Window);

  // Assemble label
  _label = new QLabel(tr("Reaction") + " " + _reaction->getDisplayName());
  _label->setFont(Application::getApp()->getH1Font());
  _label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

  // Assemble reaction equation renderer:
  _equation_view = new ReactionGraphic();
  ReactionEquationRenderer *renderer = new ReactionEquationRenderer(_reaction->getReaction());
  _equation_view->setScene(renderer);

  // Add buttons and label for parameter list
  _addParamButton = new QPushButton(tr("+"));
  _addParamButton->setToolTip(tr("Add new parameter"));
  _addParamButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  _remParamButton = new QPushButton("-");
  _remParamButton->setToolTip(tr("Delete parameter"));
  _remParamButton->setEnabled(false);
  _remParamButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  _makeGlobalButton = new QPushButton(tr("Move to Global Parameters"));
  _makeGlobalButton->setToolTip(tr("Moves the selected parameter into the global scope."));
  _makeGlobalButton->setEnabled(false);
  _makeGlobalButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  QLabel *param_label = new QLabel(tr("Local parameters"));
  param_label->setFont(Application::getApp()->getH3Font());
  param_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  param_label->setAlignment(Qt::AlignRight);

  // Assemble and configure parameter table view:
  _paramTable = new QTableView();
  _paramTable->setModel(_reaction->localParameters());
  _paramTable->setItemDelegateForColumn(1, new PixmapDelegate(_paramTable));
  _paramTable->setItemDelegateForColumn(
        2, new ExpressionDelegate(_reaction->localParameters()->kineticLaw(), _paramTable));
  _paramTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

  // Do layout
  QHBoxLayout *head_layout = new QHBoxLayout();
  head_layout->addWidget(_addParamButton, 0, Qt::AlignLeft);
  head_layout->addWidget(_remParamButton, 0, Qt::AlignLeft);
  head_layout->addWidget(_makeGlobalButton, 0, Qt::AlignLeft);
  head_layout->addWidget(param_label, 1, Qt::AlignRight);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_label, 0, Qt::AlignRight);
  layout->addWidget(_equation_view);
  layout->addLayout(head_layout);
  layout->addWidget(_paramTable);
  setLayout(layout);

  // Connect signals:
  QObject::connect(_reaction, SIGNAL(destroyed()), this, SLOT(deleteLater()));
  QObject::connect(_addParamButton, SIGNAL(clicked()), this, SLOT(onAddParamClicked()));
  QObject::connect(_remParamButton, SIGNAL(clicked()), this, SLOT(onRemParamClicked()));
  QObject::connect(_makeGlobalButton, SIGNAL(clicked()), this, SLOT(onMakeParamGlobalClicked()));
  QObject::connect(
        _paramTable->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(onSelectionChanged(QItemSelection,QItemSelection)));
  QObject::connect(_equation_view, SIGNAL(doubleClicked()), this, SLOT(onReactionEditing()));
  QObject::connect(_reaction->localParameters(), SIGNAL(identifierUpdated()),
                   this, SLOT(onParametersChanged()));
  QObject::connect(_reaction->localParameters(), SIGNAL(nameUpdated()),
                   this, SLOT(onParametersChanged()));
}


void
ReactionView::onAddParamClicked()
{
  // Forward call to parameter list model:
  _reaction->localParameters()->addParameter();
}


void
ReactionView::onRemParamClicked()
{
  // Check if an identifier of a parameter is selected:
  if (! _paramTable->selectionModel()->hasSelection()) {
    _remParamButton->setEnabled(false);
    _makeGlobalButton->setEnabled(false);
    return;
  }
  QModelIndexList indices = _paramTable->selectionModel()->selectedIndexes();
  if (1 != indices.size()) {
    _remParamButton->setEnabled(false);
    _makeGlobalButton->setEnabled(false);
    return;
  }
  QModelIndex index = indices.front();
  if (0 > index.column()) {
    _remParamButton->setEnabled(false);
    _makeGlobalButton->setEnabled(false);
    return;
  }

  // Tell species list model to remove species:
  _reaction->localParameters()->remParameter(index.row());
}


void
ReactionView::onMakeParamGlobalClicked()
{
  // Check if an identifier of a parameter is selected:
//  if (! _paramTable->selectionModel()->hasSelection()) {
//    _remParamButton->setEnabled(false);
//    _makeGlobalButton->setEnabled(false);
//    return;
//  }

  QModelIndexList indices = _paramTable->selectionModel()->selectedIndexes();
  if (1 != indices.size()) {
    _remParamButton->setEnabled(false);
    _makeGlobalButton->setEnabled(false);
    return;
  }
  QModelIndex index = indices.front();
  if (0 > index.column()) {
    _remParamButton->setEnabled(false);
    _makeGlobalButton->setEnabled(false);
    return;
  }

  // Move param into global scope:
  std::string param_id = _reaction->getReaction()->getKineticLaw()->getParameter(index.row())->getIdentifier();
  std::string reaction_id  =_reaction->getReaction()->getIdentifier();
  iNA::Ast::Model &model = *((iNA::Ast::Model *)_reaction->getReaction()->getKineticLaw()->getParentScope());
  try {
    iNA::Trafo::makeParameterGlobal(param_id, reaction_id, model);
  } catch (iNA::Exception &err) {
    // This should not happen:
    iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::ERROR);
    message << "Can not move local parameter " << param_id
            << " of reaction " << reaction_id
            << " into global scope: " << err.what();
    iNA::Utils::Logger::get().log(message);
    // Signal user:
    QMessageBox::warning(0, tr("Internal error"), tr(message.getText().c_str()));
    return;
  }

  // Tell species list model to remove species:
  _reaction->localParameters()->updateCompleteTable();
}


void
ReactionView::onSelectionChanged(const QItemSelection &selected, const QItemSelection &unselected)
{
  QModelIndexList indices = selected.indexes();
  if (1 != indices.size()) {
    _remParamButton->setEnabled(false);
    _makeGlobalButton->setEnabled(false);
    return;
  }
  QModelIndex index = indices.front();
  if (0 > index.column()) {
    _remParamButton->setEnabled(false);
    _makeGlobalButton->setEnabled(false);
    return;
  }
  // ok, allow removal of parameters:
  _remParamButton->setEnabled(true);
  _makeGlobalButton->setEnabled(true);
}


void
ReactionView::onReactionEditing()
{
  // Show reaction editor wizard for this reaction:
  ReactionEditor editor(((ModelItem *)(_reaction->parent()->parent()))->getModel(), _reaction->getReaction());
  if (QDialog::Rejected == editor.exec()) { return; }

  // Update reaction, unfortunately, the reaction gets removed and added again, such that
  // at least the reaction node needs to be resetted.
  editor.commitReactionScope();

  // Update complete view (reaction equation view and paramter list):
  ReactionEquationRenderer *renderer = new ReactionEquationRenderer(_reaction->getReaction());
  _equation_view->setScene(renderer);
  // Update local parameter list
  _reaction->localParameters()->updateCompleteTable();
  // Update reaction label:
  _label->setText(tr("Reaction") + " " + _reaction->getDisplayName());

  // Update tree model
  Application::getApp()->docTree()->resetCompleteTree();
}


void
ReactionView::onParametersChanged() {
  // Update reation equation and kinetic law view:
  ReactionEquationRenderer *renderer = new ReactionEquationRenderer(_reaction->getReaction());
  _equation_view->setScene(renderer);
}
