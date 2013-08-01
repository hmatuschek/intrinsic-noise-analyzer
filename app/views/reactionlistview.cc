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

  // Add reaction and new species to the model
  editor.commitReactionScope();

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

  // Add new reaction and new species to the model
  editor.commitReactionScope();
  // Update document tree
  Application::getApp()->resetSelectedItem();
  Application::getApp()->docTree()->resetCompleteTree();
}
