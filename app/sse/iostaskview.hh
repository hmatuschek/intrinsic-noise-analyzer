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
  explicit IOSTaskView(IOSTaskItem *task_item, QWidget *parent=0);

protected:
  virtual QWidget *createResultWidget(TaskItem *task_item);
};



class IOSResultWidget : public QWidget
{
  Q_OBJECT

public:
  explicit IOSResultWidget(IOSTaskItem *_ios_task_wrapper, QWidget *parent = 0);


private slots:
  void _onPlotButtonPressed();
  void _onGenericPlotButtonPressed();
  void _onSaveButtonPressed();
  void saveAsCSV(const QString &filename);
  void saveAsMAT(const QString &filename);

private:
  IOSTaskItem *_ios_task_wrapper;
  TableWrapper *_tableWrapper;
  QTableView   *_dataTable;
  QPushButton  *_plotButton;
  QPushButton  *_genericPlotButton;
  QPushButton  *_saveButton;
};

#endif // RETASKVIEW_HH
