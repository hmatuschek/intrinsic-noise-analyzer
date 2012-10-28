#include "versioncheck.hh"
#include <config.hh>
#include <QtNetwork/QNetworkRequest>
#include <QRegExp>
#include "../application.hh"


VersionCheck::VersionCheck(QObject *parent)
  : QObject(parent), _access(0), _reply(0)
{
  // Just allocate the network abstraction layer
  _access = new QNetworkAccessManager(this);
  // Connect to response handler:
  QObject::connect(
        _access, SIGNAL(finished(QNetworkReply*)), this, SLOT(onDataReceived(QNetworkReply*)));
}


void
VersionCheck::startCheck() {
  // Assemble and send request:
  QNetworkRequest request("http://intrinsic_noise_analyzer.googlecode.com/files/currentVersion.txt");
  request.setRawHeader(
        "User-Agent", QString("intrinsic Noise Analyzer (v%1 %2)").arg(
          INA_VERSION_STRING, Application::getApp().uuid()));
  _access->get(request);
}


void
VersionCheck::onDataReceived(QNetworkReply *reply) {
  QString version = reply->readLine();
  QRegExp regexp("([0-9]+)\.([0-9]+)\.([0-9]+)");
  if (! regexp.exactMatch(version)) { return; }
  uint major = regexp.cap(1).toUInt();
  uint minor = regexp.cap(2).toUInt();
  uint patch = regexp.cap(2).toUInt();

  if (major > INA_VERSION_MAJOR) { emit newVersionAvailable(version); }
  if (major < INA_VERSION_MAJOR) { return; }
  if (minor > INA_VERSION_MINOR) { emit newVersionAvailable(version); }
  if (minor < INA_VERSION_MINOR) { return; }
  if (patch > INA_VERSION_PATCH) { emit newVersionAvailable(version); }
}


