#ifndef LNAMODULE_HH
#define LNAMODULE_HH

#include <QObject>
#include <QAction>

#include "lnawizard.hh"
#include "../module.hh"


class LNAModule : public Module
{
  Q_OBJECT

public:
  explicit LNAModule(QObject *parent = 0);


private slots:
  void configLNA();


private:
  LNAWizard *wizard;
  QAction *lnaAct;
};

#endif // LNAMODULE_HH
