#ifndef __INA_APP_SSSCAN_MODULE_HH__
#define __INA_APP_SSSCAN_MODULE_HH__

#include <QObject>

#include "../module.hh"
#include "ssscanwizard.hh"



class ParamScanModule : public Module
{
  Q_OBJECT
public:
  explicit ParamScanModule(QObject *parent = 0);


private slots:
  void configTask();


private:
  ParamScanWizard *wizard;
  QAction *scanAction;
};

#endif
