#include "downloaddialog.hh"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QPushButton>



DownloadDialog::DownloadDialog(const QUrl &url, QWidget *parent) :
  QDialog(parent), _local_file()
{
  // Create network access:
  _network = new QNetworkAccessManager(this);

  // Create progress bar
  _progress = new QProgressBar();

  // Create hidden error message:
  _label = new QLabel();
  _label->setVisible(false);

  // Create chancel / close button.
  QPushButton *close_button = new QPushButton(tr("Cancel"));

  // Assemble layout
  QHBoxLayout *hbox = new QHBoxLayout();
  hbox->addWidget(_progress);
  hbox->addWidget(close_button);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(hbox);
  layout->addWidget(_label);
  setLayout(layout);

  // Obtain remote file name
  _remote_file_name = QFileInfo(url.path()).fileName();
  if (! _remote_file_name.isEmpty()) {
    _local_file.setFileTemplate(_remote_file_name);
  }

  // Create request and connect to future reply events:
  _response = _network->get(QNetworkRequest(url));
  QObject::connect(_response, SIGNAL(error(QNetworkReply::NetworkError)),
                   this, SLOT(onError(QNetworkReply::NetworkError)));
  QObject::connect(_response, SIGNAL(downloadProgress(qint64,qint64)),
                   this, SLOT(onProgress(qint64,qint64)));
  QObject::connect(_response, SIGNAL(finished()), this, SLOT(onSuccess()));

  // Connect button:
  QObject::connect(close_button, SIGNAL(clicked()), this, SLOT(onCancel()));
}


DownloadDialog::~DownloadDialog()
{
  _response->deleteLater();
}


QString
DownloadDialog::localFileName() const {
  return _local_file.fileName();
}

bool
DownloadDialog::hasRemoteFileName() const {
  return ! _remote_file_name.isEmpty();
}

QString
DownloadDialog::remoteFileName() const {
  return _remote_file_name;
}


void
DownloadDialog::onError(QNetworkReply::NetworkError error)
{
  _label->setText(tr("Error while downloading file."));
  _label->setVisible(true);
  _progress->setEnabled(false);
}


void
DownloadDialog::onProgress(qint64 downloaded, qint64 total)
{
  _progress->setMaximum(total);
  _progress->setValue(downloaded);
}


void
DownloadDialog::onSuccess()
{
  // Set 100%
  _progress->setValue(_progress->maximum());
  // Read all data...
  QByteArray data = _response->readAll();

  // create temporary file
  if (! _local_file.open()) {
    _label->setText(tr("Unable to open local, temorary file!"));
    _label->setVisible(true);
    return;
  }

  // Write data into temporary file and close it again:
  _local_file.write(data); _local_file.close();
  // Close dialog with success:
  accept();
}


void
DownloadDialog::onCancel()
{
  if (!_response->isFinished()) {
    // The transmission was stopped (maybe an error)
    reject(); return;
  }

  if (!_response->isRunning()) {
    // Stop transmission
    _response->abort();
    // Close dialog:
    reject(); return;
  }
}
