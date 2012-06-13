#ifndef __INA_APP_SSE_SSEMODULE_HH__
#define __INA_APP_SSE_SSEMODULE_HH__


#include <QObject>
#include <QAction>

#include "../module.hh"
#include "ssewizard.hh"


class SSEModule : public Module
{
  Q_OBJECT

public:
  explicit SSEModule(QObject *parent=0);

private slots:
  void configSSE();

private:
  SSEWizard *wizard;
  QAction   *sseAct;
};

#endif // __INA_APP_SSE_SSE_MODULE_HH__
