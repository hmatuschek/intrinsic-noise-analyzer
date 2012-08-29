#ifndef __INA_APP_VIEWS_UNITEDITOR_HH__
#define __INA_APP_VIEWS_UNITEDITOR_HH__

#include <QStackedWidget>
#include <QLabel>
#include <QLineEdit>

#include <ast/unitdefinition.hh>


/** Simple Unit display & editor widget. */
class UnitEditor : public QStackedWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit UnitEditor(QWidget *parent = 0);
  /** Constructor with unit. */
  UnitEditor(const Fluc::Ast::Unit &unit, QWidget *parent = 0);

  /** Retunrs the unit. */
  const Fluc::Ast::Unit &unit() const;
  /** Resets the unit, updates views. */
  void setUnit(const Fluc::Ast::Unit &unit);

signals:
  /** Will be emitted if the unit was changed. */
  void unitChanged();

protected:
  /** Handles double click events. */
  virtual void mouseDoubleClickEvent(QMouseEvent *event);

private slots:
  /** Will be called if the editing of the unit is finished.
   * If the unit can be parsed, a unitChanged() signal is emitted. */
  void onEditingFinished();

private:
  /** Holds the current unit. */
  Fluc::Ast::Unit _unit;
  /** The unit display widget. */
  QLabel *_unitDisplay;
  /** The unit editor widget. */
  QLineEdit *_unitEditor;

private:
  /** Internal used constructor. */
  void postConstructor();
};

#endif // __INA_APP_VIEWS_UNITEDITOR_HH__
