#ifndef __INA_APP_CONFIGURATION_HH__
#define __INA_APP_CONFIGURATION_HH__

#include <QFont>
#include <QSettings>
#include <QDateTime>


/** Provides some configurations. */
class Configuration
{
protected:
  /** Persistent setting. */
  QSettings _settings;
  /** The default (system) text-font. */
  QFont _text_font;
  /** The default font for headings level 3. */
  QFont _h3_font;
  /** The default font for headings level 2. */
  QFont _h2_font;
  /** The default font for headings level 1. */
  QFont _h1_font;


public:
  /** Constructor. */
  Configuration();

  /** Returns the H1 font. */
  const QFont &getH1Font();
  /** Returns the H2 font. */
  const QFont &getH2Font();
  /** Returns the H3 font. */
  const QFont &getH3Font();
  /** Returns the default text-font. */
  const QFont &getTextFont();

  /** Returns the list of recently imported models. */
  void recentModels(QStringList &list);
  /** Adds a recently imported model. */
  void addRecentModel(const QString &modelpath);

  /** Returns true if the periodic version check is enabled. */
  bool notifyNewVersionAvailable();
  /** Enables/Disables the periodic version check. */
  void setNotifyNewVersionAvailable(bool enabled);

  /** Returns the time & date of the last update check. */
  QDateTime lastUpdateCheck();
  /** Sets the date-time of the last update check to now. */
  void checkedForUpdate();

  /** Returns the per user unique UUID. */
  QString uuid();
};

#endif
