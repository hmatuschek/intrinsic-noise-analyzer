#include "documentitem.hh"
#include "exception.hh"
#include "../doctree/modelitem.hh"
#include "../doctree/analysesitem.hh"
#include "../application.hh"

#include <QMessageBox>
#include <QFileInfo>

using namespace iNA;



DocumentItem::DocumentItem(const QString &path, QObject *parent)
  : QObject(parent), DocumentTreeItem(), file_path(path)
{
  this->model = new ModelItem(file_path, this);
  this->model->setTreeParent(this);

  this->analyses = new AnalysesItem(this);
  this->analyses->setTreeParent(this);

  this->_children.append(this->model);
  this->_children.append(this->analyses);

  // Construct label:
  QFileInfo file_info(file_path);
  this->label
      = QString("%1 (%2)").arg(this->getModel().getName().c_str()).arg(file_info.fileName());

  // Construct context menu:
  this->closeAct = new QAction(tr("Close document"), this);
  QObject::connect(this->closeAct, SIGNAL(triggered()), this, SLOT(closeDocument()));

  this->contextMenu = new QMenu();
  this->contextMenu->addAction(this->closeAct);
}



DocumentItem::DocumentItem(iNA::Ast::Model *model, const QString &path, QObject *parent)
  : QObject(parent), DocumentTreeItem(), file_path(path)
{
  this->model = new ModelItem(model, this);
  this->model->setTreeParent(this);

  this->analyses = new AnalysesItem(this);
  this->analyses->setTreeParent(this);

  this->_children.append(this->model);
  this->_children.append(this->analyses);

  // Construct label:
  QFileInfo file_info(file_path);
  this->label
      = QString("%1 (%2)").arg(this->getModel().getName().c_str()).arg(file_info.fileName());

  // Construct context menu:
  this->closeAct = new QAction(tr("Close document"), this);
  QObject::connect(this->closeAct, SIGNAL(triggered()), this, SLOT(closeDocument()));

  this->contextMenu = new QMenu();
  this->contextMenu->addAction(this->closeAct);
}


DocumentItem::~DocumentItem()
{
  // mark Context menu for deletion:
  this->contextMenu->deleteLater();
}


iNA::Ast::Model &
DocumentItem::getModel()
{
  return this->model->getModel();
}


const iNA::Ast::Model &
DocumentItem::getModel() const
{
  return this->model->getModel();
}


void
DocumentItem::addTask(TaskItem *task)
{
  this->analyses->addTask(task);
}


bool
DocumentItem::providesContextMenu() const
{
  return true;
}


void
DocumentItem::showContextMenu(const QPoint &global_pos)
{
  this->contextMenu->popup(global_pos);
}



/* ******************************************************************************************** *
 * Implementation of the TreeItem interface.
 * ******************************************************************************************** */
const QString &
DocumentItem::getLabel() const
{
  return this->label;
}



/* ******************************************************************************************** *
 * Implementation of event callbacks:
 * ******************************************************************************************** */
void
DocumentItem::closeDocument()
{
  if (this->analyses->tasksRunning())
  {
    QMessageBox::warning(0, tr("Cannot close document"), tr("Analysis in progress."));
    return;
  }

  // Remove document from tree:
  Application::getApp()->docTree()->removeDocument(this);
  // Reset selected item:
  Application::getApp()->resetSelectedItem();

  // Mark document for deletion:
  this->deleteLater();
}
