#ifndef __INA_APP_STEADYSTATE_STEADYSTATEMODULE_HH__
#define __INA_APP_STEADYSTATE_STEADYSTATEMODULE_HH__

#include <QObject>

#include "../module.hh"
#include "steadystatewizard.hh"



class SteadyStateModule : public Module
{
  Q_OBJECT
public:
  explicit SteadyStateModule(QObject *parent = 0);


private slots:
  void configSteadyState();


private:
  SteadyStateWizard *wizard;
  QAction *steadyStateAction;
};

#endif // __INA_APP_STEADYSTATE_STEADYSTATEMODULE_HH__
