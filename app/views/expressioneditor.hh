#ifndef __INA_APP_VIEWS_EXPRESSIONEDITOR_HH__
#define __INA_APP_VIEWS_EXPRESSIONEDITOR_HH__

#include <QLineEdit>
#include <QModelIndex>
#include <QCompleter>
#include <ast/scope.hh>


/** Tiny helper widget for editing expressions. */
class ExpressionEditor : public QLineEdit
{
  Q_OBJECT

public:
  /** Trivial constructor w/o completer. */
  explicit ExpressionEditor(QWidget *parent = 0);

  /** Constructor with completer. */
  explicit ExpressionEditor(iNA::Ast::Scope &scope, QWidget *parent = 0);

protected:
  /** Handles iteration through possible completions. */
  void keyPressEvent(QKeyEvent *event);
  void idUnderCursor(QString &word);

private slots:
  void insertCompletion(const QString &completion);

private:
  QCompleter *_completer;
};

#endif // EXPRESSIONEDITOR_HH
