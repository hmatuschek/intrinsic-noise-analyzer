#ifndef __INA_APP_IMPORTMODELDIALOG_HH__
#define __INA_APP_IMPORTMODELDIALOG_HH__

#include <QDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include "fileselector.hh"


/// @deprecated Remove that class, unused.
class ModelFormatQuestion : public QDialog
{
  Q_OBJECT

public:
  typedef enum {
    SBML_MODEL,
    SBMLSH_MODEL
  } Format;

public:
  /** Asks the user for the type of the model file. */
  explicit ModelFormatQuestion(const QString &filename, QWidget *parent = 0);
  Format getFormat() const;

private:
  QComboBox *_format_selector;
  QDialogButtonBox *_buttons;
};


#endif // __INA_APP_IMPORTMODELDIALOG_HH__
