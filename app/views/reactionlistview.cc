#include "reactionlistview.hh"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include "../application.hh"
#include "reactioneditor.hh"


ReactionListView::ReactionListView(ReactionsItem *reactions, QWidget *parent)
  : QWidget(parent), _reactions(reactions->reactionList())
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
  _newReacButton->setToolTip(tr("Add a new reaction"));
  _newReacButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // rem reaction button
  _remReacButton = new QPushButton("-");
  _remReacButton->setToolTip(tr("Deletes the selected reaction"));
  _remReacButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  _remReacButton->setEnabled(false);

  // Assemble list view:
  _reactionList = new QTableView();
  _reactionList->setModel(_reactions);
  _reactionList->horizontalHeader()->setStretchLastSection(true);
  _reactionList->setSelectionMode(QAbstractItemView::SingleSelection);

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
}


void
ReactionListView::onNewReaction()
{
  // Show reaction editor wizard:
  ReactionEditor editor(_reactions->getModel());
  if (QDialog::Rejected == editor.exec()) { return; }

  // Add reaction and new species to the model
  editor.commitReactionScope();
  // Update document tree
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
  // If not the name/identifier column is selected -> quit
  if (0 != idx.column()) { return; }
  // Get the selected reaction
  iNA::Ast::Reaction *reaction = _reactions->getModel().getReaction(idx.row());

  // Last chance to quit
  if (QMessageBox::Yes != QMessageBox::question(
        0, tr("Remove reaction."),
        tr("Are you sure to delete the reaction, this can not be undone."),
        QMessageBox::Yes, QMessageBox::No)) { return; }

  _reactions->getModel().remDefinition(reaction);
  Application::getApp()->docTree()->resetCompleteTree();
}


void
ReactionListView::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deseleced)
{
  QModelIndexList indices = selected.indexes();
  if (1 != indices.size()) { _remReacButton->setEnabled(false); return; }
  QModelIndex index = indices.at(0);
  if (0 != index.column()) { _remReacButton->setEnabled(false); return; }
  _remReacButton->setEnabled(true);
}
