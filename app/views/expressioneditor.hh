#ifndef __INA_APP_VIEWS_EXPRESSIONEDITOR_HH__
#define __INA_APP_VIEWS_EXPRESSIONEDITOR_HH__

#include <QLineEdit>


/** Tiny helper widget for editing expressions. */
class ExpressionEditor : public QLineEdit
{
  Q_OBJECT

public:
  explicit ExpressionEditor(QWidget *parent = 0);

protected:
  /** Handles iteration through possible completions. */
  void keyPressEvent(QKeyEvent *event);
};

#endif // EXPRESSIONEDITOR_HH
