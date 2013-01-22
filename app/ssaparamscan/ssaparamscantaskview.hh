#ifndef __INA_APP_SSAPARAMSCAN_TASKVIEW_HH__
#define __INA_APP_SSAPARAMSCAN_TASKVIEW_HH__

#include <QWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QTableView>
#include <QLabel>

#include "ssaparamscantaskwrapper.hh"
#include "../models/tablewrapper.hh"
#include "../views/taskerrorwidget.hh"
#include "../views/taskview.hh"


/**
 * General view object for @c LNATask instances.
 */
class SSAParamScanTaskView : public TaskView
{
  Q_OBJECT

public:
  explicit SSAParamScanTaskView(SSAParamScanTaskWrapper *task_item, QWidget *parent=0);

protected:
  virtual QWidget *createResultWidget(TaskItem *task_item);
};



class SSAParamScanResultWidget : public QWidget
{
  Q_OBJECT

protected:
  SSAParamScanTaskWrapper *paramscan_task_wrapper;


public:
  explicit SSAParamScanResultWidget(SSAParamScanTaskWrapper *paramscan_task_wrapper, QWidget *parent = 0);


private slots:
  void plotButtonPressed();
  void saveButtonPressed();  
  void customPlotButtonPressed();


private:
  TableWrapper *tableWrapper;
  QTableView   *dataTable;
  QPushButton  *plotButton;
  QPushButton  *saveButton;
  QPushButton  *customPlotButton;
};

#endif // LNATASKVIEW_HH
