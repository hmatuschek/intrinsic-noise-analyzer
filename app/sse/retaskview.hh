#ifndef RETASKVIEW_HH
#define RETASKVIEW_HH

#include <QWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QTableView>
#include <QLabel>

#include "retaskwrapper.hh"
#include "../models/tablewrapper.hh"
#include "../views/taskerrorwidget.hh"
#include "../views/taskview.hh"


/**
 * General view object custom @c RETask instances.
 */
class RETaskView : public TaskView
{
  Q_OBJECT

public:
  explicit RETaskView(RETaskWrapper *task_item, QWidget *parent=0);

protected:
  virtual QWidget *createResultWidget(TaskItem *task_item);
};



class REResultWidget : public QWidget
{
  Q_OBJECT

protected:
  RETaskWrapper *re_task_wrapper;

public:
  explicit REResultWidget(RETaskWrapper *re_task_wrapper, QWidget *parent = 0);

private slots:
  void quickPlotButtonPressed();
  void genericPlotButtonPressed();
  void saveButtonPressed();

private:
  TableWrapper *tableWrapper;
  QTableView   *dataTable;
};

#endif // RETASKVIEW_HH
