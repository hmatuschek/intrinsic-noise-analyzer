#include "expressioneditor.hh"
#include <QKeyEvent>
#include <QCompleter>


ExpressionEditor::ExpressionEditor(QWidget *parent)
  : QLineEdit(parent)
{
  // pass...
}


void
ExpressionEditor::keyPressEvent(QKeyEvent *event)
{
  // Get completer, if there is no completer -> forward event
  QCompleter *completer = this->completer();
  if (0 == completer) { QLineEdit::keyPressEvent(event); return; }

  if (Qt::Key_Down == event->key()) {
    if (! completer->setCurrentRow(completer->currentRow()+1) ) { completer->setCurrentRow(0); }
    completer->complete(); return;
  }

  if (Qt::Key_Up == event->key()) {
    if (! completer->setCurrentRow(completer->currentRow()-1)) { completer->setCurrentRow(completer->model()->rowCount()-1); }
    completer->complete(); return;
  }

  if (Qt::Key_Right == event->key()) {
    // If cursor is at last position
    if (cursorPosition() == text().size()) {
      completer->complete(); return;
    }
  }

  // Forward event...
  QLineEdit::keyPressEvent(event);
}
