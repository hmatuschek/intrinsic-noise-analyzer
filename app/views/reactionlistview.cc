#include "reactionlistview.hh"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
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

  // Assemble list view:
  _reactionList = new QTableView();
  _reactionList->setModel(_reactions);
  _reactionList->horizontalHeader()->setStretchLastSection(true);

  QHBoxLayout *header_layout = new QHBoxLayout();
  header_layout->addWidget(_newReacButton, 0, Qt::AlignLeft);
  header_layout->addWidget(label, 1, Qt::AlignRight);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(header_layout);
  layout->addWidget(_reactionList,1);
  setLayout(layout);

  QObject::connect(reactions, SIGNAL(destroyed()), this, SLOT(deleteLater()));
  QObject::connect(_newReacButton, SIGNAL(clicked()), this, SLOT(onNewReaction()));
}


void
ReactionListView::onNewReaction() {
  ReactionCreator editor(_reactions->getModel());
  editor.exec();
}
