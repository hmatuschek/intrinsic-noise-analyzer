#ifndef __INA_APP_MODELS_SPECIESSELECTIONMODEL_HH__
#define __INA_APP_MODELS_SPECIESSELECTIONMODEL_HH__

#include <QAbstractTableModel>
#include <QStringList>
#include <QVector>

// Forward declarations
namespace iNA { namespace Ast { class Model; } }

/** Implements a simple table model that allows to select a set of species and displays the
 * species names with tinyTeX. */
class SpeciesSelectionModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit SpeciesSelectionModel(iNA::Ast::Model *model, QObject *parent = 0);

  /** Returns a list of species identifiers that are selected. */
  QStringList selectedSpecies();

  /** Returns the number of species. */
  virtual int rowCount(const QModelIndex &parent) const;
  /** Returns the number of columns. */
  virtual int columnCount(const QModelIndex &parent) const;
  /** Retunrs the falgs. */
  virtual Qt::ItemFlags flags(const QModelIndex &index) const;
  /** Returns the item data or check state. */
  virtual QVariant data(const QModelIndex &index, int role) const;
  /** Changes checkstate. */
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

signals:
  /** Gets emitted once the selection has been changed. */
  void selectionChanged();

public slots:
  /** Selects all species in the list. */
  void selectAllSpecies();
  /** Unselects all species in the list. */
  void selectNoSpecies();
  /** Inverts the selection. */
  void invertSelection();

protected:
  /** Holds a weak reference to the model. */
  iNA::Ast::Model *_model;
  /** The set of selected species. */
  QVector<bool> _selection;
};

#endif // SPECIESSELECTIONMODEL_HH
