#ifndef __INA_APP_SSAPARAMSCAN_TASKWRAPPER_HH__
#define __INA_APP_SSAPARAMSCAN_TASKWRAPPER_HH__

#include "../doctree/taskitem.hh"
#include "../doctree/plotitem.hh"
#include "ssaparamscantask.hh"


/** This class represents the analysis or results of the analysis as a tree item for the side panel.
 * @todo Rename to item. */
class SSAParamScanTaskWrapper : public TaskItem
{
  Q_OBJECT

public:
  SSAParamScanTaskWrapper(SSAParamScanTask *task, QObject *parent=0);

  SSAParamScanTask *getParamScanTask();

  virtual bool providesView() const;
  virtual QWidget *createView();
};


#endif
