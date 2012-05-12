#ifndef __INA_APP_RE_REMODULE_HH__
#define __INA_APP_RE_REMODULE_HH__


#include <QObject>
#include <QAction>

#include "../module.hh"
#include "rewizard.hh"


class REModule : public Module
{
  Q_OBJECT

public:
  explicit REModule(QObject *parent=0);

private slots:
  void configRE();

private:
  REWizard *wizard;
  QAction  *reAct;
};

#endif // __INA_APP_RE_REMODULE_HH__
