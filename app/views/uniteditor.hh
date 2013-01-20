#ifndef __INA_APP_VIEWS_UNITEDITOR_HH__
#define __INA_APP_VIEWS_UNITEDITOR_HH__

#include <QStackedWidget>
#include <QLabel>
#include <QLineEdit>
#include "unitrenderer.hh"

#include <ast/unitdefinition.hh>


/** A trivial display widget for rendered units. */
class UnitDisplayWidget: public QLabel
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit UnitDisplayWidget(const iNA::Ast::Unit &unit, QWidget *parent=0);

  /** Updates the unit. */
  void setUnit(const iNA::Ast::Unit &unit);

protected:
  /** Renders the unit. */
  virtual void paintEvent(QPaintEvent *event);

private:
  /** Holds the unit renderer for the current unit. */
  UnitRenderer _renderer;
};


/** Simple Unit display & editor widget. */
class UnitEditor : public QStackedWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit UnitEditor(QWidget *parent = 0);
  /** Constructor with unit. */
  UnitEditor(const iNA::Ast::Unit &unit, QWidget *parent = 0);

  /** Retunrs the unit. */
  const iNA::Ast::Unit &unit() const;
  /** Resets the unit, updates views. */
  void setUnit(const iNA::Ast::Unit &unit);

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
  iNA::Ast::Unit _unit;
  /** The unit display widget. */
  UnitDisplayWidget *_unitDisplay;
  /** The unit editor widget. */
  QLineEdit *_unitEditor;

private:
  /** Internal used constructor. */
  void postConstructor();
};

#endif // __INA_APP_VIEWS_UNITEDITOR_HH__
