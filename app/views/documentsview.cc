#include "documentsview.hh"
#include "../doctree/documenttree.hh"
#include "../models/application.hh"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileInfo>
#include "downloaddialog.hh"



/* ********************************************************************************************* *
 * Implementation of SBML Models View, A QTreeView with some nice functionality
 * ********************************************************************************************* */
DocumentsView::DocumentsView(QWidget *parent)
  : QTreeView(parent)
{
  // The QAbstractItemModel holding all the data is the application singleton:
  this->setModel(Application::getApp()->docTree());

  // Enable context menus:
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  QObject::connect(this, SIGNAL(customContextMenuRequested(QPoint)),
                   this, SLOT(conextMenuRequested(QPoint)));

  // Enable drop events:
  setAcceptDrops(true);
}


void
DocumentsView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
  // First call default handler:
  QTreeView::selectionChanged(selected, deselected);

  // Anyway reset the selected item in application:
  Application::getApp()->resetSelectedItem();

  // try to find out, which item was selected:
  if (0 == selected.indexes().size()) { return; }
  QModelIndex idx = selected.indexes().front();
  if (! idx.isValid()) { return; }

  // Signal Application to open the index
  Application::getApp()->itemSelected(idx);
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


void
DocumentsView::dragEnterEvent(QDragEnterEvent *event)
{
  // Check possible actions:
  if (0 == (event->possibleActions() & Qt::CopyAction)) {
    return;
  }

  // Check text, if text is an url -> handle
  if (event->mimeData()->hasText()) {
    // Try to parse text as URL
    QUrl url(event->mimeData()->text(), QUrl::StrictMode);
    if (! url.isValid()) { return; }
    // Of successful -> accept
    event->setDropAction(Qt::CopyAction);
    event->accept();
    return;
  }

  // We only accept one URL:
  if (event->mimeData()->hasUrls() && (1 ==  event->mimeData()->urls().size()) ) {
    event->setDropAction(Qt::CopyAction);
    event->accept();
    return;
  }
}


void
DocumentsView::dragMoveEvent(QDragMoveEvent *event)
{
  event->acceptProposedAction();
}

void
DocumentsView::dragLeaveEvent(QDragLeaveEvent *event)
{
  event->accept();
}


void
DocumentsView::dropEvent(QDropEvent *event)
{
  // Check action
  if (0 == (event->possibleActions() & Qt::CopyAction)) { return; }

  // Will hold the URL of the local or remote file to open:
  QUrl url;

  // Check if drop event is list of URLs or a string with an url:
  if (event->mimeData()->hasUrls() && (1 == event->mimeData()->urls().size())) {
    url = event->mimeData()->urls().front();
    event->setDropAction(Qt::CopyAction);
    event->accept();
  } else if (event->mimeData()->hasText()) {
    url = QUrl(event->mimeData()->text(), QUrl::StrictMode);
    if (! url.isValid()) { return; }
    event->setDropAction(Qt::CopyAction);
    event->accept();
  } else {
    return;
  }

  // First, handle local files:
  if ("file"==url.scheme() || ""==url.scheme()) {
    Application::getApp()->importModel(url.path(), false);
    return;
  }

  // Handle remote files:
  DownloadDialog dialog(url);
  if (QDialog::Accepted != dialog.exec()) { return; }

  // Open downloaded file
  QString local_file = dialog.localFileName();
  // Try to determine file type by extension or remote file name:
  QFileInfo info(dialog.remoteFileName());
  Application::ModelType type=Application::FORMAT_ASK_USER;
  if (("xml"==info.suffix()) || ("sbml"==info.suffix())) { type = Application::FORMAT_SBML; }
  else if (("mod"==info.suffix()) || ("sbmlsh"==info.suffix())) { type=Application::FORMAT_SBMLsh; }
  Application::getApp()->importModel(local_file, true, type);
}
