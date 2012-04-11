#ifndef LNASTEADYSTATEMODULE_HH
#define LNASTEADYSTATEMODULE_HH

#include <QObject>

#include "../module.hh"
#include "lnasteadystatewizard.hh"



class LNASteadyStateModule : public Module
{
  Q_OBJECT
public:
  explicit LNASteadyStateModule(QObject *parent = 0);


private slots:
  void configSteadyState();


private:
  LNASteadyStateWizard *wizard;
  QAction *steadyStateAction;
};

#endif // LNASTEADYSTATEMODULE_HH
