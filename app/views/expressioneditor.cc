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
  QCompleter *completer = this->completer();
  if (0 == completer) { QLineEdit::keyPressEvent(event); return; }

  if (Qt::Key_Down == event->key()) {
    if (! completer->setCurrentRow(completer->currentRow()+1) ) { completer->setCurrentRow(0); }
    completer->complete();
  } else {
    QLineEdit::keyPressEvent(event);
  }
}
