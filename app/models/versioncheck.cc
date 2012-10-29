#include "versioncheck.hh"
#include <config.hh>
#include <QtNetwork/QNetworkRequest>
#include <QRegExp>
#include "../application.hh"
#include <config.hh>


VersionCheck::VersionCheck(QObject *parent)
  : QObject(parent), _access(0)
{
  // Just allocate the network abstraction layer
  _access = new QNetworkAccessManager(this);
  // Connect to response handler:
  QObject::connect(
        _access, SIGNAL(finished(QNetworkReply*)), this, SLOT(onDataReceived(QNetworkReply*)));
}


void
VersionCheck::startCheck()
{
  // Skip if periodic check is disabled by compiler flag;
#ifdef INA_DISABLE_NEW_VERSION_CHECK
  return;
#endif

  // Skip if periodic check is disabled by user config.
  if (! Application::getApp()->checkNewVersionAvailable()) { return; }

  // Skip update if last update was less than 7 days ago:
  QDateTime last_check = Application::getApp()->lastUpdateCheck();
  if (last_check.addDays(7) < QDateTime::currentDateTime()) { return; }

  // Assemble user agent ID
  QString version = INA_VERSION_STRING;
  QString system = "unkown";
#ifdef Q_WS_X11
  system = "X11";
#elif defined(Q_WS_MAC)
  system = "MacOS X";
#elif defined(Q_WS_WIN)
  system = "Windows";
#endif
  QString user_agent = QString("iNA/%1 (%2)").arg(version).arg(system);

  // Assemble and send request:
  QNetworkRequest request(QUrl("http://intrinsic_noise_analyzer.googlecode.com/files/currentVersion.txt"));
  request.setRawHeader("User-Agent", user_agent.toAscii());
  _access->get(request);
}


void
VersionCheck::onDataReceived(QNetworkReply *reply) {
  // Update last update check date:
  Application::getApp()->checkedForUpdate();

  // Read current published version number:
  QString version = reply->readLine();
  QRegExp regexp("([0-9]+)\\.([0-9]+)\\.([0-9]+)");
  if (! regexp.exactMatch(version)) { return; }
  uint major = regexp.cap(1).toUInt();
  uint minor = regexp.cap(2).toUInt();
  uint patch = regexp.cap(2).toUInt();

  // Emit "newVersionAvailable" if this version number is smaller
  if (major > INA_VERSION_MAJOR) { emit newVersionAvailable(version); }
  if (major < INA_VERSION_MAJOR) { return; }
  if (minor > INA_VERSION_MINOR) { emit newVersionAvailable(version); }
  if (minor < INA_VERSION_MINOR) { return; }
  if (patch > INA_VERSION_PATCH) { emit newVersionAvailable(version); }
}


