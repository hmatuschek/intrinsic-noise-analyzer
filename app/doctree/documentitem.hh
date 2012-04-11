#ifndef __FLUC_GUI_DOCUMENT_WRAPPER_HH__
#define __FLUC_GUI_DOCUMENT_WRAPPER_HH__

#include <QObject>
#include <sbml/SBMLTypes.h>

#include "documenttreeitem.hh"
#include "ast/model.hh"
#include <QMenu>
#include <QAction>

class TaskItem;
class ModelItem;
class AnalysesItem;


/**
 * Represents an open SBML document/model in the application.
 *
 * A document has a SBML model assigned held in the @c model attribute. Furthermore, there is
 * a list of analyses applied on the model.
 *
 * @ingroup gui
 */
class DocumentItem : public QObject, public DocumentTreeItem
{
  Q_OBJECT

protected:
  /**
   * Holds the path to the SBML file, used to identify the document.
   */
  QString file_path;

  /**
   * Holds an instance of the parsed SBML model.
   *
   * @deprecated Use the Ast::Model or Models::BaseModel instead.
   */
  libsbml::SBMLDocument *document;

  /**
   * Holds the instance of the model-wrapper, representing the SBML model for the application.
   */
  ModelItem *model;

  /**
   * Holds a list of analyses performed on the model.
   */
  AnalysesItem  *analyses;


public:
  /**
   * Constructor.
   *
   * The constructor will parse the given SBML file and constructs an @c Models::BaseModel instance.
   *
   * @param path Specifies the path of the SBML file to be parsed.
   * @param parent Specifies the "logical" parent of the instance.
   */
  explicit DocumentItem(const QString &path, QObject *parent=0);

  /**
   * Destructor.
   *
   * Also frees the SBML document held in @c document.
   */
  ~DocumentItem();

  /**
   * Returns the SBML model of the SBML document.
   *
   * @deprecated Will be removed soon.
   */
  libsbml::Model *getSBMLModel();

  /**
   * Returns the @c Ast::Model instance associated with this document.
   */
  Fluc::Ast::Model *getModel();

  /**
   * Returns the @c Ast::Model instance associated with this document.
   */
  const Fluc::Ast::Model *getModel() const;

  /**
   * Adds a task to the document.
   *
   * The task will be shown as an analysis of the model.
   */
  void addTask(TaskItem *task);

  virtual bool providesContextMenu() const;

  virtual void showContextMenu(const QPoint &global_pos);


  /* ******************************************************************************************* *
   * Implementation of the Wrapper interface.
   * ******************************************************************************************* */
  virtual const QString &getLabel() const;


  /* ******************************************************************************************* *
   * Callbacks for the context menu:
   * ******************************************************************************************* */
private slots:
  void onDocumentClose();


  /* ******************************************************************************************* *
   * Some private data:
   * ******************************************************************************************* */
private:
  QAction *closeAct;
  QMenu   *contextMenu;
  QString label;
};



#endif // __FLUC_GUI_DOCUMENT_WRAPPER_HH__
