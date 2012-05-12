#ifndef __INA_APP_IOS_IOSMODULE_HH__
#define __INA_APP_IOS_IOSMODULE_HH__


#include <QObject>
#include <QAction>

#include "../module.hh"
#include "ioswizard.hh"


class IOSModule : public Module
{
  Q_OBJECT

public:
  explicit IOSModule(QObject *parent=0);

private slots:
  void configIOS();

private:
  IOSWizard *wizard;
  QAction   *iosAct;
};

#endif // __INA_APP_IOS_IOS_MODULE_HH__
