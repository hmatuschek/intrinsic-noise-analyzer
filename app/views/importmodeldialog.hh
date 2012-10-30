#ifndef __INA_APP_IMPORTMODELDIALOG_HH__
#define __INA_APP_IMPORTMODELDIALOG_HH__

#include <QDialog>
#include <QComboBox>
#include <QDialogButtonBox>


/** This simple dialog asks the user for the model format for import. The dialog is used to
 * if the format can not be inferred by the file extension. */
class ModelFormatQuestion : public QDialog
{
  Q_OBJECT

public:
  /** Defines possible model formats. */
  typedef enum {
    SBML_MODEL,
    SBMLSH_MODEL
  } Format;

public:
  /** Asks the user for the type of the model file. */
  explicit ModelFormatQuestion(const QString &filename, QWidget *parent = 0);
  /** Returns the selected format of the model. */
  Format getFormat() const;

private:
  /** The format selector. */
  QComboBox *_format_selector;
  /** Dialog buttons. */
  QDialogButtonBox *_buttons;
};


#endif // __INA_APP_IMPORTMODELDIALOG_HH__
