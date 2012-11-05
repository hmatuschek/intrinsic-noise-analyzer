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

  QLabel *details = new QLabel(
        tr("You are currently using iNA version <b>%1</b>. The recent version <b>%2</b> is "
           "available for <a href=\"http://code.google.com/p/intrinsic-noise-analyzer/downloads/list\">download</a>.").arg(INA_VERSION_STRING).arg(version));
  details->setAlignment(Qt::AlignHCenter);

  _checkEnabled = new QCheckBox("Notify about new versions");
  _checkEnabled->setChecked(true);

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(heading, 0);
  layout->addWidget(details, 1);
  layout->addWidget(_checkEnabled);
  layout->addWidget(buttons);
  setLayout(layout);

  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(onAccept()));
}


void
NewVersionDialog::onAccept() {
  // disable checks if not wanted:
  Application::getApp()->setNotifyNewVersionAvailable(_checkEnabled->isChecked());
  accept();
}
