#ifndef SSATASKWRAPPER_HH
#define SSATASKWRAPPER_HH

#include "../doctree/taskitem.hh"
#include "../models/tablewrapper.hh"
#include "ssatask.hh"


class SSATaskWrapper : public TaskItem
{
  Q_OBJECT

protected:
  TableWrapper *table_wrapper;

public:
  SSATaskWrapper(SSATask *task, QObject *parent=0);

  virtual bool providesView() const;

  virtual QWidget *createView();

  TableWrapper *getTimeSeries();

  SSATask *getSSATask();
};

#endif // SSATASKWRAPPER_HH
