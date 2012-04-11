#ifndef LNASTEADYSTATEVIEW_HH
#define LNASTEADYSTATEVIEW_HH

#include <QWidget>
#include <QLabel>
#include <QTableView>
#include <QTableWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QTextStream>

#include "lnasteadystatetaskwrapper.hh"
#include "../views/taskview.hh"


class LNASteadyStateView : public TaskView {
public:
  explicit LNASteadyStateView(LNASteadyStateTaskWrapper *task_item, QWidget *parent=0);
};



class LNASteadyStateResultWidget : public QWidget
{
  Q_OBJECT

protected:
  LNASteadyStateTaskWrapper *ss_task_wrapper;


public:
  explicit LNASteadyStateResultWidget(LNASteadyStateTaskWrapper *task_wrapper, QWidget *parent = 0);


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
  QLabel       *cov_label;
  QTableWidget *covariance_view;
  QLabel       *spec_label;
  QTableView   *spectrum_view;
  QPushButton  *spec_plot_button;
  QPushButton  *spec_save_button;
  QPushButton  *data_save_button;
};

#endif // LNASTEADYSTATEVIEW_HH
