#include "sbmlsheditordialog.hh"

#include <QTextEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QMessageBox>
#include "../models/sbmlshhighlighter.hh"

#include <parser/sbmlsh/sbmlsh.hh>
#include <exception.hh>
#include <parser/exception.hh>


SbmlshEditorDialog::SbmlshEditorDialog(QWidget *parent) :
  QDialog(parent), _editor(0), _document(0), _model(0)
{
  setModal(true);

  _editor = new QTextEdit();
  _editor->setLineWrapMode(QTextEdit::NoWrap);
  _editor->setAcceptRichText(false);

  _document = new QTextDocument(_editor);
  _editor->setDocument(_document);
  _sbmlshHighlighter = new SbmlshHighlighter(_document);

  QDialogButtonBox *buttons
      = new QDialogButtonBox(QDialogButtonBox::Cancel|QDialogButtonBox::Open, Qt::Horizontal);

  // Assemble layout:
  QVBoxLayout *main_layout = new QVBoxLayout();
  main_layout->setContentsMargins(0,0,0,5);
  _editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  main_layout->addWidget(_editor);
  main_layout->addWidget(buttons);

  setLayout(main_layout); resize(600, 640);

  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(onReimport()));
  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}


void
SbmlshEditorDialog::setModel(iNA::Ast::Model &model)
{
  std::stringstream buffer;
  iNA::Parser::Sbmlsh::exportModel(model, buffer);
  _document->setPlainText(buffer.str().c_str());
}


void
SbmlshEditorDialog::onReimport()
{
  // Serialize text into std::stream:
  std::stringstream buffer; buffer << _document->toPlainText().toStdString();

  bool has_error=false;

  // parse model...
  try {
    _model = iNA::Parser::Sbmlsh::importModel(buffer);
  } catch (iNA::Parser::ParserError &err) {
    QList<QTextEdit::ExtraSelection> selections;
    QTextEdit::ExtraSelection selection;
    selection.custommat.setBackground(QColor(Qt::red).lighter(160));
    selection.custommat.setProperty(QTextcustommat::FullWidthSelection, true);
    selection.cursor = _editor->textCursor();
    selection.cursor.movePosition(QTextCursor::Start);
    selection.cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, err.line()-1);
    selection.cursor.clearSelection();
    selections.append(selection); _editor->setExtraSelections(selections);
    has_error = true;
    QMessageBox::critical(0, tr("Error while parsing SBML-sh."), err.what());
  } catch (iNA::Exception &err) {
    QMessageBox::critical(0, tr("Error while parsing SBML-sh."), err.what());
    has_error = true;
  }

  if (!has_error) accept();
}


iNA::Ast::Model *
SbmlshEditorDialog::takeModel() {
  iNA::Ast::Model *model = _model; _model = 0;
  return model;
}
