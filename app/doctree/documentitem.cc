#include "documentitem.hh"
#include "exception.hh"
#include "../doctree/modelitem.hh"
#include "../doctree/analysesitem.hh"
#include "../application.hh"

#include <QMessageBox>


using namespace Fluc;



DocumentItem::DocumentItem(const QString &path, QObject *parent)
  : QObject(parent), DocumentTreeItem(), file_path(path), document(0)
{
  // Read SBML document from file:
  this->document = libsbml::readSBMLFromFile(path.toStdString().c_str());

  // Check for errors:
  if (0 != this->document->getNumErrors())
  {
    SBMLParserError err;
    err << "Can not parse SBML file " << path.toStdString()
        << ": " << this->document->getError(0)->getMessage();
    throw err;
  }

  // Convert to l2v4:
  if (! this->document->setLevelAndVersion(2,4))
  {
    SBMLParserError err;
    err << "The model in " << path.toStdString()
        << " is not compatible with SBML leven 2 version 4!";
    throw err;
  }

  this->model = new ModelItem(this->document, this);
  this->model->setTreeParent(this);

  this->analyses = new AnalysesItem(this);
  this->analyses->setTreeParent(this);

  this->_children.append(this->model);
  this->_children.append(this->analyses);

  // Construct label:
  this->label
      = QString("%1 (%2)").arg(this->getModel()->getName().c_str()).arg(this->file_path);

  // Construct context menu:
  this->closeAct = new QAction(tr("close document"), this);
  QObject::connect(this->closeAct, SIGNAL(triggered()), this, SLOT(onDocumentClose()));

  this->contextMenu = new QMenu();
  this->contextMenu->addAction(this->closeAct);
}



DocumentItem::~DocumentItem()
{
  // Free the SBML document.
  delete this->document;

  // mark Context menu for deletion:
  this->contextMenu->deleteLater();
}


libsbml::Model *
DocumentItem::getSBMLModel()
{
  return this->document->getModel();
}


Fluc::Ast::Model *
DocumentItem::getModel()
{
  return this->model->getModel();
}


const Fluc::Ast::Model *
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
 * Implementation of the Wrapper interface.
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
DocumentItem::onDocumentClose()
{
  if (this->analyses->tasksRunning())
  {
    QMessageBox::warning(0, tr("Can not close document"), tr("There are analyses in grogress."));

    return;
  }

  // Remove document from tree:
  Application::getApp()->docTree()->removeDocument(this);

  // mark document for deletion:
  this->deleteLater();
}
