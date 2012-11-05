#include "reactionview.hh"
#include "../models/application.hh"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QTableView>


#include "reactioneditor.hh"
#include "../doctree/modelitem.hh"
#include "reactionequationrenderer.hh"
#include "../models/expressiondelegate.hh"

ReactionView::ReactionView(ReactionItem *reaction, QWidget *parent) :
    QWidget(parent), _reaction(reaction)
{
  // Basic layout
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  setBackgroundRole(QPalette::Window);

  // Assemble label
  QLabel *label = new QLabel(tr("Reaction") + " " + _reaction->getDisplayName());
  label->setFont(Application::getApp()->getH1Font());
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

  // Assemble reaction equation renderer:
  _equation_view = new ReactionGraphic();
  ReactionEquationRenderer *renderer = new ReactionEquationRenderer(_reaction->getReaction());
  _equation_view->setScene(renderer);

  // Add buttons and label custom parameter list
  _addParamButton = new QPushButton(tr("+"));
  _addParamButton->setToolTip(tr("Add new parameter"));
  _addParamButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  _remParamButton = new QPushButton("-");
  _remParamButton->setToolTip(tr("Delete parameter"));
  _remParamButton->setEnabled(false);
  _remParamButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  QLabel *param_label = new QLabel(tr("Local parameters"));
  param_label->setFont(Application::getApp()->getH3Font());
  param_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  param_label->setAlignment(Qt::AlignRight);

  // Assemble and configure parameter table view:
  _paramTable = new QTableView();
  _paramTable->setModel(_reaction->localParameters());
  _paramTable->setItemDelegatecustomColumn(1, new PixmapDelegate(_paramTable));
  _paramTable->setItemDelegatecustomColumn(
        2, new ExpressionDelegate(_reaction->localParameters()->kineticLaw(), _paramTable));

  // Do layout
  QHBoxLayout *head_layout = new QHBoxLayout();
  head_layout->addWidget(_addParamButton, 0, Qt::AlignLeft);
  head_layout->addWidget(_remParamButton, 0, Qt::AlignLeft);
  head_layout->addWidget(param_label, 1, Qt::AlignRight);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(label, 0, Qt::AlignRight);
  layout->addWidget(_equation_view);
  layout->addLayout(head_layout);
  layout->addWidget(_paramTable);
  setLayout(layout);

  // Connect signals:
  QObject::connect(_reaction, SIGNAL(destroyed()), this, SLOT(deleteLater()));
  QObject::connect(_addParamButton, SIGNAL(clicked()), this, SLOT(onAddParamClicked()));
  QObject::connect(_remParamButton, SIGNAL(clicked()), this, SLOT(onRemParamClicked()));
  QObject::connect(
        _paramTable->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(onSelectionChanged(QItemSelection,QItemSelection)));
  QObject::connect(_equation_view, SIGNAL(doubleClicked()), this, SLOT(onReactionEditing()));

}


void
ReactionView::onAddParamClicked()
{
  // customward call to parameter list model:
  _reaction->localParameters()->addParameter();
}


void
ReactionView::onRemParamClicked()
{
  // Check if an identifier of a parameter is selected:
  if (! _paramTable->selectionModel()->hasSelection()) {
    _remParamButton->setEnabled(false);
    return;
  }
  QModelIndexList indices = _paramTable->selectionModel()->selectedIndexes();
  if (1 != indices.size()) { _remParamButton->setEnabled(false); return; }
  QModelIndex index = indices.front();
  if (0 != index.column()) { _remParamButton->setEnabled(false); return; }

  // Tell species list model to remove species:
  _reaction->localParameters()->remParameter(index.row());
}


void
ReactionView::onSelectionChanged(const QItemSelection &selected, const QItemSelection &unselected)
{
  QModelIndexList indices = selected.indexes();
  if (1 != indices.size()) { _remParamButton->setEnabled(false); return; }
  QModelIndex index = indices.front();
  if (0 != index.column()) { _remParamButton->setEnabled(false); return; }
  // ok, allow removal of parameters:
  _remParamButton->setEnabled(true);
}

void
ReactionView::onReactionEditing()
{
  // Show reaction editor wizard custom this reaction:
  ReactionEditor editor(((ModelItem *)(_reaction->parent()->parent()))->getModel(), _reaction->getReaction());
  if (QDialog::Rejected == editor.exec()) { return; }

  // Add new reaction and new species to the model
  editor.commitReactionScope();

  // Update complete view (reaction equation view and paramter list):
  ReactionEquationRenderer *renderer = new ReactionEquationRenderer(_reaction->getReaction());
  _equation_view->setScene(renderer);
  _reaction->localParameters()->updateCompleteTable();
}
