#include "documentsview.hh"
#include "../application.hh"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>



/* ********************************************************************************************* *
 * Implementation of SBML Models View, A QTreeView with some nice functionality
 * ********************************************************************************************* */
DocumentsView::DocumentsView(QWidget *parent) :
    QTreeView(parent)
{
  // The QAbstractItemModel holding all the data is the application singleton:
  this->setModel(Application::getApp()->docTree());

  // Enable context menus:
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  QObject::connect(this, SIGNAL(customContextMenuRequested(QPoint)),
                   this, SLOT(conextMenuRequested(QPoint)));
}


void
DocumentsView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
  // First call default handler:
  QTreeView::selectionChanged(selected, deselected);

  // try to find out, which item was selected:
  if (0 == selected.indexes().size())
  {
    return;
  }

  QModelIndex idx = selected.indexes().front();
  if (! idx.isValid())
  {
    return;
  }

  // Signal Application to open the index
  Application::getApp()->openModelIndex(idx);
}


void
DocumentsView::conextMenuRequested(const QPoint &pos)
{
  // Get model index:
  QModelIndex index = this->indexAt(pos);

  if (! index.isValid()) {
    return;
  }

  Application::getApp()->showContextMenuAt(index, this->mapToGlobal(pos));
}



/* ********************************************************************************************* *
 * Implementation of SBML Models View, A QTreeView with some nice functionality
 * ********************************************************************************************* */
DocumentsSidePanel::DocumentsSidePanel(QWidget *parent)
  : QWidget(parent)
{
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setContentsMargins(0,0,0,0);

  this->documents_view = new DocumentsView();
  this->documents_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  layout->addWidget(this->documents_view);

  this->setLayout(layout);
}
