#ifndef LNATASKWRAPPER_HH
#define LNATASKWRAPPER_HH

#include "../doctree/taskitem.hh"
#include "../doctree/plotitem.hh"
#include "lnatask.hh"



class LNATaskItem : public TaskItem
{
  Q_OBJECT

public:
  LNATaskItem(LNATask *task, QObject *parent=0);

  LNATask *getLNATask();

  virtual bool providesView() const;
  virtual QWidget *createView();
};


#endif // LNATASKWRAPPER_HH
