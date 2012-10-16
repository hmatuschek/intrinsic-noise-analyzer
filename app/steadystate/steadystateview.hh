#ifndef __INA_APP_STEADYSTATE_LNASTEADYSTATEVIEW_HH__
#define __INA_APP_STEADYSTATE_LNASTEADYSTATEVIEW_HH__

#include <QWidget>
#include <QLabel>
#include <QTableView>
#include <QTableWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QTextStream>

#include "steadystatetaskwrapper.hh"
#include "../views/taskview.hh"


class SteadyStateView : public TaskView {
public:
  explicit SteadyStateView(SteadyStateTaskWrapper *task_item, QWidget *parent=0);

protected:
  virtual QWidget *createResultWidget(TaskItem *task_item);
};



class SteadyStateResultWidget : public QWidget
{
  Q_OBJECT

protected:
  SteadyStateTaskWrapper *ss_task_wrapper;


public:
  explicit SteadyStateResultWidget(SteadyStateTaskWrapper *task_wrapper, QWidget *parent = 0);


private slots:
  void plotSpectrum();
  void saveSpectrum();
  void taskStateChanged();
  void saveData();

private:
  void setValues();

private:
  QLabel       *state_label;
  QTableWidget *state_view;
  QLabel       *lna_cov_label;
  QTableWidget *lna_covariance_view;
  QLabel       *ios_cov_label;
  QTableWidget *ios_covariance_view;
  QLabel       *spec_label;
  QTableView   *spectrum_view;
  QPushButton  *spec_plot_button;
  QPushButton  *spec_save_button;
  QPushButton  *data_save_button;
};

#endif
