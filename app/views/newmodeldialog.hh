#ifndef __INA_APP_VIEWS_NEWMODELDIALOG_HH__
#define __INA_APP_VIEWS_NEWMODELDIALOG_HH__

#include <QDialog>
#include <QLineEdit>


/** Trivial dialog to ask for an identifier and a name for the new model. */
class NewModelDialog : public QDialog
{
  Q_OBJECT

public:
  explicit NewModelDialog(QWidget *parent = 0);

  QString getModelName() const;
  QString getModelIdentifier() const;

private:
  QLineEdit *_modelIdentifier;
  QLineEdit *_modelName;
};

#endif // NEWMODELDIALOG_HH
