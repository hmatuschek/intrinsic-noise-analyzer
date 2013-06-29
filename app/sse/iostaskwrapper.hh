#ifndef IOSTASKWRAPPER_HH
#define IOSTASKWRAPPER_HH

#include "../doctree/taskitem.hh"
#include "../doctree/plotitem.hh"
#include "iostask.hh"


class IOSTaskItem : public TaskItem
{
  Q_OBJECT

public:
  IOSTaskItem(IOSTask *task, QObject *parent=0);

  IOSTask *getIOSTask();

  virtual bool providesView() const;
  virtual QWidget *createView();
};


#endif // LNATASKWRAPPER_HH
