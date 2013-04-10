#ifndef __FLUC_GUI_DOCUMENT_WRAPPER_HH__
#define __FLUC_GUI_DOCUMENT_WRAPPER_HH__

#include <QObject>

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
  /** Holds the path to the SBML file, used to identify the document. */
  QString _file_path;
  /** Holds the instance of the model-wrapper, representing the SBML model for the application. */
  ModelItem *_model;
  /** Holds a list of analyses performed on the model. */
  AnalysesItem  *_analyses;


public:
  /** Constructor.
   * The constructor will parse the given SBML file and constructs an @c Models::BaseModel instance.
   *
   * @todo Unify constructors.
   *
   * @param path Specifies the path of the SBML file to be parsed.
   * @param parent Specifies the "logical" parent of the instance.
   */
  explicit DocumentItem(const QString &path, QObject *parent=0);

  /** Constructor.
   * The constructor will parse the given SBML file and constructs an @c Models::BaseModel instance.
   *
   * @param model The parsed Model.
   * @param path Specifies the path of the file.
   * @param parent Specifies the "logical" parent of the instance.
   */
  explicit DocumentItem(iNA::Ast::Model *_model, const QString &path=QString(), QObject *parent=0);

  /** Destructor. */
  virtual ~DocumentItem();

  /** Returns the @c Ast::Model instance associated with this document. */
  iNA::Ast::Model &getModel();
  /** Returns the @c Ast::Model instance associated with this document. */
  const iNA::Ast::Model &getModel() const;

  QModelIndex indexOfAnalysesItem() const;
  AnalysesItem *analysesItem();
  size_t numAnalyses() const;

  /** Adds a task to the document.
   * The task will be shown as an analysis of the model. */
  void addTask(TaskItem *task);

  /** Returns true. */
  virtual bool providesContextMenu() const;
  /** Assembles a context menu. */
  virtual void showContextMenu(const QPoint &global_pos);


  /* ******************************************************************************************* *
   * Implementation of the DocumentTreeItem interface.
   * ******************************************************************************************* */
  /** Returns the label for this item. */
  virtual const QString &getLabel() const;

  /** Updates the document item. */
  void updateItemData();

  /* ******************************************************************************************* *
   * Callbacks for the context menu:
   * ******************************************************************************************* */
public slots:
  /** Removes the document from the tree. */
  void closeDocument();


  /* ******************************************************************************************* *
   * Some private data:
   * ******************************************************************************************* */
private:
  QAction *_closeAct;
  QMenu   *_contextMenu;
  QString _label;
};



#endif // __FLUC_GUI_DOCUMENT_WRAPPER_HH__
