#ifndef LNATASKVIEW_HH
#define LNATASKVIEW_HH

#include <QWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QTableView>
#include <QLabel>

#include "lnataskwrapper.hh"
#include "../models/tablewrapper.hh"
#include "../views/taskerrorwidget.hh"
#include "../views/taskview.hh"


/**
 * General view object for @c LNATask instances.
 */
class LNATaskView : public TaskView
{
  Q_OBJECT

public:
  explicit LNATaskView(LNATaskWrapper *task_item, QWidget *parent=0);

protected:
  virtual QWidget *createResultWidget(TaskItem *task_item);
};



class LNAResultWidget : public QWidget
{
  Q_OBJECT

protected:
  LNATaskWrapper *lna_task_wrapper;


public:
  explicit LNAResultWidget(LNATaskWrapper *lna_task_wrapper, QWidget *parent = 0);


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
