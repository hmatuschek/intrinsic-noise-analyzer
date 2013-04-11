#include "documentitem.hh"
#include "documenttree.hh"
#include "exception.hh"
#include "modelitem.hh"
#include "analysesitem.hh"
#include "../models/application.hh"

#include <QMessageBox>
#include <QFileInfo>

using namespace iNA;



DocumentItem::DocumentItem(const QString &path, QObject *parent)
  : QObject(parent), DocumentTreeItem(), _file_path(path)
{
  _model = new ModelItem(_file_path, this);
  addChild(_model);

  _analyses = new AnalysesItem(this);
  addChild(_analyses);

  // Construct context menu:
  _closeAct = new QAction(tr("Close document"), this);
  _contextMenu = new QMenu();
  _contextMenu->addAction(_closeAct);
  QObject::connect(_closeAct, SIGNAL(triggered()), this, SLOT(closeDocument()));

  // Update item label:
  updateItemData();
}



DocumentItem::DocumentItem(iNA::Ast::Model *model, const QString &path, QObject *parent)
  : QObject(parent), DocumentTreeItem(), _file_path(path)
{
  _model = new ModelItem(model, this);
  addChild(_model);

  _analyses = new AnalysesItem(this);
  addChild(_analyses);

  // Construct context menu:
  _closeAct = new QAction(tr("Close document"), this);
  QObject::connect(this->_closeAct, SIGNAL(triggered()), this, SLOT(closeDocument()));

  _contextMenu = new QMenu();
  _contextMenu->addAction(this->_closeAct);

  // Update label
  updateItemData();
}


DocumentItem::~DocumentItem() {
  // mark Context menu for deletion:
  _contextMenu->deleteLater();
}


iNA::Ast::Model &
DocumentItem::getModel() {
  return _model->getModel();
}


const iNA::Ast::Model &
DocumentItem::getModel() const {
  return _model->getModel();
}


AnalysesItem *
DocumentItem::analysesItem() {
  return _analyses;
}

ReactionsItem *
DocumentItem::reactionsItem() {
  return _model->reactionsItem();
}

size_t
DocumentItem::numAnalyses() const {
  return _analyses->getTreeChildCount();
}

void
DocumentItem::addTask(TaskItem *task) {
  _analyses->addTask(task);
}


bool
DocumentItem::providesContextMenu() const {
  return true;
}


void
DocumentItem::showContextMenu(const QPoint &global_pos) {
  _contextMenu->popup(global_pos);
}



/* ******************************************************************************************** *
 * Implementation of the TreeItem interface.
 * ******************************************************************************************** */
const QString &
DocumentItem::getLabel() const {
  return _label;
}

void
DocumentItem::updateItemData() {
  // Update item label:
  if (0 != _model) {
    QString model_name = _model->getModel().getIdentifier().c_str();
    if (_model->getModel().hasName()) { model_name = _model->getModel().getName().c_str(); }
    if (0 == _file_path.size())  {
      _label = model_name;
    } else {
      QFileInfo info(_file_path);
      _label = QString("%1 (%2)").arg(model_name).arg(info.fileName());
    }
  }

  // signal doctree to update the data.
  Application::getApp()->docTree()->markForUpdate(this);
}



/* ******************************************************************************************** *
 * Implementation of event callbacks:
 * ******************************************************************************************** */
void
DocumentItem::closeDocument() {
  if (this->_analyses->tasksRunning()) {
    QMessageBox::warning(0, tr("Cannot close document"), tr("Analysis in progress."));
    return;
  }

  // Reset selected item:
  Application::getApp()->resetSelectedItem();
  // Remove document from tree:
  Application::getApp()->docTree()->removeDocument(this);
  // Mark document for deletion:
  this->deleteLater();
}
