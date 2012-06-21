#ifndef IOSASKVIEW_HH
#define IOSASKVIEW_HH

#include <QWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QTableView>
#include <QLabel>

#include "iostaskwrapper.hh"
#include "../models/tablewrapper.hh"
#include "../views/taskerrorwidget.hh"
#include "../views/taskview.hh"


/**
 * General view object for @c IOSTask instances.
 */
class IOSTaskView : public TaskView
{
  Q_OBJECT

public:
  explicit IOSTaskView(IOSTaskWrapper *task_item, QWidget *parent=0);

protected:
  virtual QWidget *createResultWidget(TaskItem *task_item);
};



class IOSResultWidget : public QWidget
{
  Q_OBJECT

protected:
  IOSTaskWrapper *ios_task_wrapper;


public:
  explicit IOSResultWidget(IOSTaskWrapper *ios_task_wrapper, QWidget *parent = 0);


private slots:
  void plotButtonPressed();
  void saveButtonPressed();


private:
  TableWrapper *tableWrapper;
  QTableView   *dataTable;
  QPushButton  *plotButton;
  QPushButton  *saveButton;
};

#endif // RETASKVIEW_HH
