#ifndef __INA_APP_VERSIONCHECK_HH__
#define __INA_APP_VERSIONCHECK_HH__

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>


/** Trivial class that checks if the current version of iNA is still up to date. */
class VersionCheck : public QObject
{
  Q_OBJECT

public:
  /** Constructor, does nothing yet. */
  VersionCheck(QObject *parent=0);

  /** Starts a version check request, if a new version is found, the @c newVersionAvailable signal
   * will be emitted. */
  void startCheck();

signals:
  /** This signal will be emitted if there is a new version available. */
  void newVersionAvailable(QString new_version);

private slots:
  /** Handles responses. */
  void onDataReceived(QNetworkReply *reply);

private:
  /** Represents the network abstraction layer. */
  QNetworkAccessManager *_access;
};

#endif
