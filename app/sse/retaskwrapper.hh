#ifndef RETASKWRAPPER_HH
#define RETASKWRAPPER_HH

#include "../doctree/taskitem.hh"
#include "../doctree/plotitem.hh"
#include "retask.hh"


class RETaskItem : public TaskItem
{
  Q_OBJECT

public:
  RETaskItem(RETask *task, QObject *parent=0);

  RETask *getRETask();

  virtual bool providesView() const;
  virtual QWidget *createView();
};


#endif // LNATASKWRAPPER_HH
