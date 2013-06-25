#include "expressioneditor.hh"
#include <QKeyEvent>
#include <QCompleter>
#include <QAbstractItemView>
#include <QAbstractProxyModel>
#include "../models/scopeitemmodel.hh"


// Tiny help function to check if a char is a valid identifier char
inline bool isIdChar(const QChar &c) {
  return c.isLetterOrNumber() || (c == '_');
}


ExpressionEditor::ExpressionEditor(QWidget *parent)
  : QLineEdit(parent), _completer(0)
{
  // pass...
}

ExpressionEditor::ExpressionEditor(iNA::Ast::Scope &scope, QWidget *parent)
  : QLineEdit(parent)
{
  _completer = new QCompleter(this);
  _completer->setCompletionMode(QCompleter::PopupCompletion);
  _completer->setModel(new ScopeItemModel(scope, _completer));
  _completer->setWidget(this);
  QObject::connect(_completer, SIGNAL(activated(QString)),
                   this, SLOT(insertCompletion(QString)));
}

void
ExpressionEditor::insertCompletion(const QString &completion) {
  // Insert identifier into expression text
  QString expr=text();
  expr.insert(cursorPosition(), completion.mid(_completer->completionPrefix().length()));
  setText(expr);
}

void
ExpressionEditor::idUnderCursor(QString &word) {
  word.clear(); int idx = cursorPosition()-1;
  while ((idx >= 0) && isIdChar(text().at(idx))) {
    word.prepend(text().at(idx--));
  }
}

void
ExpressionEditor::keyPressEvent(QKeyEvent *event)
{
  // First, handle key event...
  QLineEdit::keyPressEvent(event);

  // Get completer, if there is no completer -> done
  if (0 == _completer) { return; }

  // Start completion on "->" key
  if (_completer->popup() && (! _completer->popup()->isVisible()) && (Qt::Key_Right == event->key()) ) {
    QString prefix; idUnderCursor(prefix);
    _completer->setCompletionPrefix(prefix);
    _completer->complete();
  }

  // Complete anyway if there are at least 3 chars under the cursor:
  QString prefix; idUnderCursor(prefix);
  if (1 <= prefix.length()) {
    _completer->setCompletionPrefix(prefix);
    _completer->complete();
  }
}
