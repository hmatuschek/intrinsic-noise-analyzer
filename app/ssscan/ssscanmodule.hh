#ifndef __INA_APP_SSSCAN_MODULE_HH__
#define __INA_APP_SSSCAN_MODULE_HH__

#include <QObject>

#include "../module.hh"
#include "ssscanwizard.hh"



class SSScanModule : public Module
{
  Q_OBJECT
public:
  explicit SSScanModule(QObject *parent = 0);


private slots:
  void configTask();


private:
  SSScanWizard *wizard;
  QAction *scanAction;
};

#endif
