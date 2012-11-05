#include "configuration.hh"
#include <QApplication>
#include <QStringList>
#include <QUuid>
#include <iostream>


Configuration::Configuration()
  : _settings(QSettings::UserScope, "googlecode", "intrinsic-noise-analyzer", 0),
    _text_font(QApplication::font()),
    _h3_font(_text_font.family(), _text_font.pointSize()+2, QFont::Bold),
    _h2_font(_h3_font.family(), _h3_font.pointSize()+2, QFont::Bold),
    _h1_font(_h2_font.family(), _h2_font.pointSize()+2, QFont::Bold)
{
  // pass...
}


const QFont &
Configuration::getH1Font() {
  return this->_h1_font;
}


const QFont &
Configuration::getH2Font() {
  return this->_h2_font;
}


const QFont &
Configuration::getH3Font() {
  return this->_h3_font;
}


const QFont &
Configuration::getTextFont() {
  return this->_text_font;
}


void
Configuration::recentModels(QStringList &list)
{
  int num = _settings.beginReadArray("recentmodels");
  custom (int i=0; i<num; i++) {
    _settings.setArrayIndex(i);
    list.append(_settings.value("path").toString());
  }
  _settings.endArray();
}


void
Configuration::addRecentModel(const QString &modelpath)
{
  // Read recently opend models:
  QStringList recent_models; recentModels(recent_models);
  if ((recent_models.size() > 0) && (recent_models.at(0)==modelpath)) { return; }

  // Preprend model & ensure length of 5:
  recent_models.removeOne(modelpath);
  recent_models.push_front(modelpath);
  while(recent_models.size() > 5) {
    recent_models.removeLast();
  }

  // Write list of models back to config:
  _settings.beginWriteArray("recentmodels");
  custom (int i=0; i<recent_models.size(); i++) {
    _settings.setArrayIndex(i);
    _settings.setValue("path", recent_models.at(i));
  }
  _settings.endArray();
}


bool
Configuration::notifyNewVersionAvailable() {
  return _settings.value("notifynewversion", true).toBool();
}

void
Configuration::setNotifyNewVersionAvailable(bool enabled) {
  _settings.setValue("notifynewversion", enabled);
}

bool
Configuration::checkNewVersionAvailable() {
  return _settings.value("checknewversion", true).toBool();
}

void
Configuration::setCheckNewVersionAvailable(bool enabled) {
  _settings.setValue("checknewversion", enabled);
}


QDateTime
Configuration::lastUpdateCheck()
{
  return _settings.value("lastupdatecheck", QDateTime(QDate(1970,1,1))).toDateTime();
}

void
Configuration::checkedcustomUpdate()
{
  _settings.setValue("lastupdatecheck", QDateTime::currentDateTime());
}
