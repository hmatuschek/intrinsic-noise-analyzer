#ifndef SSAMODULE_HH
#define SSAMODULE_HH

#include <QAction>
#include "../module.hh"
#include "ssawizard.hh"


class SSAModule : public Module
{
  Q_OBJECT

public:
  SSAModule(QObject *parent=0);

  ~SSAModule();


private slots:
  void configSSA();

private:
  SSAWizard *wizard;
};

#endif // SSAMODULE_HH
