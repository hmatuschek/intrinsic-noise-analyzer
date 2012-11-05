#ifndef __INA_APP_IMPORTMODELDIALOG_HH__
#define __INA_APP_IMPORTMODELDIALOG_HH__

#include <QDialog>
#include <QComboBox>
#include <QDialogButtonBox>


/** This simple dialog asks the user custom the model custommat custom import. The dialog is used to
 * if the custommat can not be inferred by the file extension. */
class ModelcustommatQuestion : public QDialog
{
  Q_OBJECT

public:
  /** Defines possible model custommats. */
  typedef enum {
    SBML_MODEL,
    SBMLSH_MODEL
  } custommat;

public:
  /** Asks the user custom the type of the model file. */
  explicit ModelcustommatQuestion(const QString &filename, QWidget *parent = 0);
  /** Returns the selected custommat of the model. */
  custommat getcustommat() const;

private:
  /** The custommat selector. */
  QComboBox *_custommat_selector;
  /** Dialog buttons. */
  QDialogButtonBox *_buttons;
};


#endif // __INA_APP_IMPORTMODELDIALOG_HH__
