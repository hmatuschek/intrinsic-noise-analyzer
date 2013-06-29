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
  /** Returns the ID or parts of it under the cursor. */
  void idUnderCursor(QString &word);

private slots:
  /** Callback that inserts the completion at the current cursor position. */
  void insertCompletion(const QString &completion);
  /** Gets called once the editing is finished, hides the completer if visible. */
  void onEditingFinished();

private:
  /** Holds the auto-completion. */
  QCompleter *_completer;
};

#endif
