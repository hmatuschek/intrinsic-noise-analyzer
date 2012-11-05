#ifndef __INA_APP_SBMLSHEDITORDIALOG_HH__
#define __INA_APP_SBMLSHEDITORDIALOG_HH__

#include <QDialog>
#include <QTextDocument>
#include <QSyntaxHighlighter>
#include <ast/model.hh>


/** Trivial editor dialog custom SBML-sh.
 * Consists of an editor with SBML-sh syntax highlighting. */
class SbmlshEditorDialog : public QDialog
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit SbmlshEditorDialog(QWidget *parent = 0);

  /** Serializes the @c Ast::Model as SBML-sh and sets the code to the editor. */
  void setModel(iNA::Ast::Model &model);

  /** Takes the parsed Ast::Model instance from the dialog. The ownership is tranferred to the
   * caller.*/
  iNA::Ast::Model *takeModel();

private slots:
  /** "reimport" handler, parses the SBML-sh model and constructs a Ast::Model from it. */
  void onReimport();

private:
  /** Holds the editor widget. */
  QTextEdit *_editor;
  /** Holds the document of the SBML-sh code. */
  QTextDocument *_document;
  /** Holds an instance of the highlighter. */
  QSyntaxHighlighter *_sbmlshHighlighter;
  /** Holds the parsed Ast::Model (once "load" is clicked). */
  iNA::Ast::Model *_model;
};

#endif // __INA_APP_SBMLSHEDITORDIALOG_HH__
