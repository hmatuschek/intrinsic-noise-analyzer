#ifndef __INA_APP_VIEWS_NEWVERSIONDIALOG_HH__
#define __INA_APP_VIEWS_NEWVERSIONDIALOG_HH__

#include <QDialog>
#include <QCheckBox>


/** A trivial dialog to notify the user about a new version of iNA. */
class NewVersionDialog : public QDialog
{
  Q_OBJECT

public:
  /** Constructor, version = new version. */
  explicit NewVersionDialog(const QString &version, QWidget *parent = 0);
  
private slots:
  /** Enables/Disables automatic check and closes the dialog. */
  void onAccept();

private:
  /** A simple check box to diable checks. */
  QCheckBox *_checkEnabled;
};

#endif // NEWVERSIONDIALOG_HH
