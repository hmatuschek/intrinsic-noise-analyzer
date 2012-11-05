#ifndef __INA_APP_PARAMSCAN_TASKVIEW_HH__
#define __INA_APP_PARAMSCAN_TASKVIEW_HH__

#include <QWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QTableView>
#include <QLabel>

#include "paramscantaskwrapper.hh"
#include "../models/tablewrapper.hh"
#include "../views/taskerrorwidget.hh"
#include "../views/taskview.hh"


/**
 * General view object custom @c LNATask instances.
 */
class ParamScanTaskView : public TaskView
{
  Q_OBJECT

public:
  explicit ParamScanTaskView(ParamScanTaskWrapper *task_item, QWidget *parent=0);

protected:
  virtual QWidget *createResultWidget(TaskItem *task_item);
};



class ParamScanResultWidget : public QWidget
{
  Q_OBJECT

protected:
  ParamScanTaskWrapper *paramscan_task_wrapper;


public:
  explicit ParamScanResultWidget(ParamScanTaskWrapper *paramscan_task_wrapper, QWidget *parent = 0);


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
