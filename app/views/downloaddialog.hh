#ifndef __INA_APP_VIEWS_DOWNLOADDIALOG_HH__
#define __INA_APP_VIEWS_DOWNLOADDIALOG_HH__

#include <QUrl>
#include <QWidget>
#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QTemporaryFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>


/** A simple dialog showing the download progress and allows to stop the download. */
class DownloadDialog : public QDialog
{
  Q_OBJECT

public:
  /** Constructor, starts to download the file immediately. */
  explicit DownloadDialog(const QUrl &url, QWidget *parent=0);
  /** Destructor. */
  virtual ~DownloadDialog();

  /** Retruns the path to the local temporary file. The file will be deleted once the
   * dialog is destroyed. */
  QString localFileName() const;
  /** Returns true, if the remote filename is known. */
  bool hasRemoteFileName() const;
  /** Retruns the remote file name, if no remote filename is known, an empty string is returned. */
  QString remoteFileName() const;

private slots:
  /** Signal handler for network errors. */
  void onError(QNetworkReply::NetworkError error);
  /** Callback for transmission progress. */
  void onProgress(qint64 downloaded, qint64 total);
  /** Callback if the transmission was successful. */
  void onSuccess();
  /** Callback for chancel button. */
  void onCancel();

private:
  /** The download progress. */
  QProgressBar *_progress;
  /** Initially hidden error message. */
  QLabel *_label;
  /** The network. */
  QNetworkAccessManager *_network;
  /** The request reply. */
  QNetworkReply *_response;
  /** The local temporary file. */
  QTemporaryFile _local_file;
  /** The remote file name (if given). */
  QString _remote_file_name;
};

#endif // __INA_APP_VIEWS_DOWNLOADDIALOG_HH__
