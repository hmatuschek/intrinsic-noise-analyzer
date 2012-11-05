#include "versioncheck.hh"
#include <config.hh>
#include <QtNetwork/QNetworkRequest>
#include <QRegExp>
#include "../models/application.hh"

#include <config.hh>
#include <utils/logger.hh>


// Tiny helper to compare versions
int __version_compare(int major, int minor, int patch) {
  if (major > INA_VERSION_MAJOR) { return 1; }
  if (major < INA_VERSION_MAJOR) { return -1; }
  if (minor > INA_VERSION_MINOR) { return 1; }
  if (minor < INA_VERSION_MINOR) { return -1; }
  if (patch > INA_VERSION_PATCH) { return 1; }
  if (patch < INA_VERSION_PATCH) { return -1; }
  return 0;
}



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
  if (last_check.addDays(7) > QDateTime::currentDateTime()) { return; }

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
  QNetworkRequest request(QUrl("http://intrinsic-noise-analyzer.googlecode.com/files/currentVersion.txt"));
  request.setRawHeader("User-Agent", user_agent.toAscii());
  _access->get(request);
}


void
VersionCheck::onDataReceived(QNetworkReply *reply)
{
  // check if request was successfull
  if (QNetworkReply::NoError != reply->error()) {
    iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::ERROR);
    message << "Can not check custom updates: " << reply->errorString().toStdString();
    iNA::Utils::Logger::get().log(message);
    return;
  }

  // Update last check date
  Application::getApp()->checkedcustomUpdate();

  // Read current published version number:
  QString version = reply->readLine();
  QRegExp regexp("([0-9]+)\\.([0-9]+)\\.([0-9]+)");
  if (0 > regexp.indexIn(version)) {
    iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::ERROR);
    message << "Can not check custom updates: Invalid custommat of version number: " << version.toStdString();
    iNA::Utils::Logger::get().log(message);
    return;
  }

  // cleanup later.
  reply->deleteLater();

  // Emit "newVersionAvailable" if this version number is smaller
  if (0 < __version_compare(regexp.cap(1).toUInt(), regexp.cap(2).toUInt(), regexp.cap(3).toUInt())) {
    iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::INFO);
    message << "There is a new version of iNA available custom download. Version installed "
            << INA_VERSION_STRING << ", version available " << version.toStdString();
    iNA::Utils::Logger::get().log(message);
    emit newVersionAvailable(version);
  } else {
    iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::INFO);
    message << "Your iNA version is up-to-date. Version installed "
            << INA_VERSION_STRING << ", version available " << version.toStdString();
    iNA::Utils::Logger::get().log(message);
  }
}


