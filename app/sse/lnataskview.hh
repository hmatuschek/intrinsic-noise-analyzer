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
  explicit LNATaskView(LNATaskItem *task_item, QWidget *parent=0);

protected:
  virtual QWidget *createResultWidget(TaskItem *task_item);
};



class LNAResultWidget : public QWidget
{
  Q_OBJECT

public:
  explicit LNAResultWidget(LNATaskItem *_lna_task_wrapper, QWidget *parent = 0);


private slots:
  void _plotButtonPressed();
  void _genericPlotButtonPressed();
  void _saveButtonPressed();
  void saveAsCSV(const QString &filename);
  void saveAsMAT(const QString &filename);

private:
  LNATaskItem *_lna_task_wrapper;
  TableWrapper   *_tableWrapper;
  QTableView     *_dataTable;
  QPushButton    *_plotButton;
  QPushButton    *_genericPlotButton;
  QPushButton    *_saveButton;
};

#endif // LNATASKVIEW_HH
