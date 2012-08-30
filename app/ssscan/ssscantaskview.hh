#ifndef __INA_APP_SSSCAN_TASKVIEW_HH__
#define __INA_APP_SSSCAN_TASKVIEW_HH__

#include <QWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QTableView>
#include <QLabel>

#include "ssscantaskwrapper.hh"
#include "../models/tablewrapper.hh"
#include "../views/taskerrorwidget.hh"
#include "../views/taskview.hh"


/**
 * General view object for @c LNATask instances.
 */
class SSScanTaskView : public TaskView
{
  Q_OBJECT

public:
  explicit SSScanTaskView(SSScanTaskWrapper *task_item, QWidget *parent=0);

protected:
  virtual QWidget *createResultWidget(TaskItem *task_item);
};



class SSScanResultWidget : public QWidget
{
  Q_OBJECT

protected:
  SSScanTaskWrapper *ssscan_task_wrapper;


public:
  explicit SSScanResultWidget(SSScanTaskWrapper *ssscan_task_wrapper, QWidget *parent = 0);


private slots:
  void plotButtonPressed();
  void saveButtonPressed();


private:
  TableWrapper *tableWrapper;
  QTableView   *dataTable;
  QPushButton  *plotButton;
  QPushButton  *saveButton;
};

#endif // LNATASKVIEW_HH
