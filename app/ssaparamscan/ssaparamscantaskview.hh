#ifndef __INA_APP_SSAPARAMSCAN_TASKVIEW_HH__
#define __INA_APP_SSAPARAMSCAN_TASKVIEW_HH__

#include <QWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QTableView>
#include <QLabel>
#include <QListWidget>

#include "ssaparamscantaskwrapper.hh"
#include "../models/tablewrapper.hh"
#include "../views/taskerrorwidget.hh"
#include "../views/taskview.hh"


/** General view object for @c SSAParamScanTask instances.
 */
class SSAParamScanTaskView : public TaskView
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit SSAParamScanTaskView(SSAParamScanTaskWrapper *task_item, QWidget *parent=0);

protected:
  /** Returns the result widget. */
  virtual QWidget *createResultWidget(TaskItem *task_item);
  /** Returns the preview widget. The widget gets updated at each time-step of the simulation. */
  virtual QWidget *createProgressWidget(TaskItem *task_item);
};


/** Displays a preview of the current statistics. The you can coose to continue the simulation. */
class SSAParamScanPreviewWidget : public QWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit SSAParamScanPreviewWidget(SSAParamScanTaskWrapper *taks_wrapper, QWidget *parent=0);

private slots:
  void onDone();
  void onItemChanged(QListWidgetItem *item);
  void onScheduleUpdatePlot();
  void updatePlot();
  void onSelectAllSpecies();
  void onSelectNoSpecies();
  void onInvertSelection();

private:
  /** Holds a weak refernce to the task item. */
  SSAParamScanTaskWrapper *_task_item;
  /** Shows a parameter scan plot for the selected species. */
  Plot::Canvas *_plot_canvas;
  /** The list widget for species selection. */
  QListWidget *_species_list;
  /** Timer event to update the plot. This timer is used to slowdown the updating to once a second. */
  QTimer _updateTimer;
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
