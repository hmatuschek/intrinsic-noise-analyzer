#include "newversiondialog.hh"
#include <config.hh>
#include <QLabel>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include "../models/application.hh"


NewVersionDialog::NewVersionDialog(const QString &version, QWidget *parent)
  : QDialog(parent)
{
  setWindowTitle(tr("New version available"));
  QLabel *heading = new QLabel("<b>A new version of iNA is available.</b>");
  heading->setAlignment(Qt::AlignCenter);

  QLabel *details = new QLabel();
  details->setText(
        tr("You are currenlty using iNA version %1, but version %2 is "
           "available for <a href=\"http://intrinsic-noise-analyzer.googlecode.com/files/list\">download</a>.").arg(INA_VERSION_STRING).arg(version));
  details->setAlignment(Qt::AlignHCenter);

  _checkEnabled = new QCheckBox("periodically check for updates");

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(heading, 0);
  layout->addWidget(details, 1);
  layout->addWidget(_checkEnabled);
  layout->addWidget(buttons);

  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(onAccept()));
}


void
NewVersionDialog::onAccept() {
  // disable checks if not wanted:
  Application::getApp()->setNotifyNewVersionAvailable(_checkEnabled->isChecked());
  accept();
}
