#ifndef RETASKWRAPPER_HH
#define RETASKWRAPPER_HH

#include "../doctree/taskitem.hh"
#include "../doctree/plotitem.hh"
#include "retask.hh"


class RETaskWrapper : public TaskItem
{
  Q_OBJECT

public:
  RETaskWrapper(RETask *task, QObject *parent=0);

  RETask *getRETask();

  virtual bool providesView() const;
  virtual QWidget *createView();
};


#endif // LNATASKWRAPPER_HH
