#ifndef SSAVIEW_HH
#define SSAVIEW_HH

#include <QWidget>
#include <QTableView>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>

#include "ssataskwrapper.hh"
#include "../views/taskview.hh"




class SSATaskView : public TaskView
{
  Q_OBJECT

public:
  explicit SSATaskView(SSATaskWrapper *task_item, QWidget *parent=0);

protected:
  virtual QWidget *createResultWidget(TaskItem *task_item);
};



class SSAResultWidget : public QWidget
{
  Q_OBJECT

protected:
  SSATaskWrapper *ssa_task_wrapper;


public:
  explicit SSAResultWidget(SSATaskWrapper *wrapper, QWidget *parent = 0);


private slots:
  void showPlot();
  void saveData();
  void _genericPlotButtonPressed();

private:
  QTableView   *data_view;
  QPushButton  *plot_button;
  QPushButton  *save_button;
  QPushButton    *_genericPlotButton;

};

#endif // SSAVIEW_HH
