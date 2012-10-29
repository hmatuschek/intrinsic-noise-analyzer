#ifndef __INA_APP_EXPORTMODELDIALOG_HH__
#define __INA_APP_EXPORTMODELDIALOG_HH__

#include <QDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include "fileselector.hh"


/**
 * Simple QDialog to export a model (SBMl, SBML-SH, ...)
 *
 * @deprecated Remove that class. Unused.
 */
class ExportModelDialog : public QDialog
{
  Q_OBJECT

public:
  typedef enum {
    SBML_MODEL,
    SBMLSH_MODEL
  } Format;

public:
  explicit ExportModelDialog(QWidget *parent = 0);

  QString getFileName() const;
  Format getFormat() const;

private slots:
  void __on_filename_selected(QString filename);

private:
  FileSelector *_file_selector;
  QComboBox *_format_selector;
  QDialogButtonBox *_buttons;
};

#endif // __INA_APP_IMPORTMODELDIALOG_HH__
