#include "taskview.hh"
#include "../models/application.hh"
#include <QVBoxLayout>
#include <QPushButton>



/* ********************************************************************************************* *
 * Implementation of TaskView
 * ********************************************************************************************* */
TaskView::TaskView(TaskItem *task_wrapper, QWidget *parent)
  : QWidget(parent), task_item(task_wrapper), current_main_widget(0)
{
  // Some basic layout stuff
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  this->setBackgroundRole(QPalette::Window);

  // Construct widgets:
  this->title = new QLabel(task_wrapper->getTask()->getLabel());
  this->title->setFont(Application::getApp()->getH1Font());
  this->title->setAlignment(Qt::AlignRight);
  this->title->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

  // Select visible widget by state of task:
  current_main_widget = new QLabel("Err, specialized TaskView has not implemented view-update.");

  // construct layout:
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(this->title);
  layout->addWidget(current_main_widget);
  this->setLayout(layout);

  // Connect signals to task:
  QObject::connect(this->task_item->getTask(), SIGNAL(stateChanged()), this, SLOT(taskStateChanged()));
  QObject::connect(this->task_item->getTask(), SIGNAL(destroyed()), this, SLOT(taskDestroyed()));
}


TaskView::~TaskView()
{
  // Pass...
}


void
TaskView::taskStateChanged()
{
  // Select visible widget by state of task:
  switch (task_item->getTask()->getState())
  {
  case Task::INITIALIZED:
  case Task::RUNNING:
    setMainWidget(createProgressWidget(task_item));
    break;

  case Task::DONE:
    setMainWidget(createResultWidget(task_item));
    break;

  case Task::ERROR:
    setMainWidget(createErrorWidget(task_item));
    break;

  case Task::TERMINATING:
    setMainWidget(createTerminatingWidget(task_item));
    break;
  }
}


void
TaskView::taskDestroyed()
{
  // The view is invalid if the task gets destroyed:
  this->deleteLater();
}


QWidget *
TaskView::createProgressWidget(TaskItem *task_item) {
  return new TaskProgressWidget(task_item);
}


QWidget *
TaskView::createErrorWidget(TaskItem *task_item) {
  return new TaskErrorWidget(task_item);
}

QWidget *
TaskView::createTerminatingWidget(TaskItem *task_item) {
  return new TaskTerminatingWidget();
}


void
TaskView::setMainWidget(QWidget *widget)
{
  if (0 != current_main_widget && widget != current_main_widget) {
    this->layout()->removeWidget(current_main_widget);
    delete current_main_widget;
    this->layout()->addWidget(widget);
  }

  current_main_widget = widget;
}



/* ********************************************************************************************* *
 * Implementation of TaskTerminatingWidget, just shows a message
 * ********************************************************************************************* */
TaskTerminatingWidget::TaskTerminatingWidget(QWidget *parent)
  : QLabel(tr("Waiting custom thread to terminate..."), parent)
{
  this->setFont(Application::getApp()->getH2Font());
}



/* ********************************************************************************************* *
 * Implementation of TaskProgressWidget
 * ********************************************************************************************* */
TaskProgressWidget::TaskProgressWidget(TaskItem *task, QWidget *parent)
  : QWidget(parent), task_item(task)
{
  // Create widget
  this->progress_bar = new QProgressBar();
  this->progress_bar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

  QHBoxLayout *time_layout = new QHBoxLayout();
  double dt = this->task_item->getTask()->getElapsedTime();
  this->time_elapsed = new QLabel(tr("Time elapsed: %1").arg(this->custommatTime(dt)));
  this->time_elapsed->setAlignment(Qt::AlignCenter);
  this->time_elapsed->setTextcustommat(Qt::LogText);
  this->time_elapsed->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  this->time_remain = new QLabel(tr("Time remaining: unknown"));
  this->time_remain->setAlignment(Qt::AlignCenter);
  this->time_remain->setTextcustommat(Qt::LogText);
  this->time_remain->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  time_layout->addWidget(this->time_elapsed);
  time_layout->addWidget(this->time_remain);

  QPushButton *stop_button = new QPushButton(tr("Stop analysis"));
  stop_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Create layout:
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addStretch();
  layout->addWidget(this->progress_bar);
  layout->addLayout(time_layout);
  layout->addStretch();
  layout->addWidget(stop_button);
  this->setLayout(layout);

  _updateTimer = new QTimer(this);
  QObject::connect(_updateTimer, SIGNAL(timeout()), this, SLOT(taskProgress()));
  _updateTimer->start(1000);

  // Connect events:
  QObject::connect(this->task_item->getTask(), SIGNAL(updateProgress()), this, SLOT(taskProgress()));
  QObject::connect(this->task_item->getTask(), SIGNAL(stateChanged()), this, SLOT(taskStateChanged()));
  QObject::connect(stop_button, SIGNAL(clicked()), this, SLOT(stopAnalysis()));

  // Update current progress:
  if (Task::INITIALIZED == this->task_item->getTask()->getState()) {
    this->progress_bar->setRange(0,0);
  } else {
    this->progress_bar->setRange(0,100);
  }
  this->progress_bar->setValue(100 * this->task_item->getTask()->getProgress());
}


void
TaskProgressWidget::taskProgress()
{
  double dt = this->task_item->getTask()->getElapsedTime();
  double p = this->task_item->getTask()->getProgress();

  this->progress_bar->setValue(100 * p);
  this->time_elapsed->setText(tr("Time elapsed: %1").arg(this->custommatTime(dt)));

  if (0 == dt || 0 == p)
  {
    this->time_remain->setText(tr("Time remaining: unknown"));
  }
  else
  {
    double time_r = (1. - p) * dt/p;
    this->time_remain->setText(tr("Time remaining: %1").arg(this->custommatTime(time_r)));
  }
}


void
TaskProgressWidget::taskStateChanged()
{
  if (Task::INITIALIZED == this->task_item->getTask()->getState()) {
    this->progress_bar->setRange(0,0);
    this->taskProgress();
  } else {
    this->progress_bar->setRange(0,100);
    this->taskProgress();
  }

  if ( (Task::INITIALIZED != this->task_item->getTask()->getState()) &&
       (Task::RUNNING != this->task_item->getTask()->getState()) ) {
    _updateTimer->stop();
  }
}


void
TaskProgressWidget::stopAnalysis()
{
  if (Task::RUNNING == this->task_item->getTask()->getState() ||
      Task::INITIALIZED == this->task_item->getTask()->getState())
  {
    this->task_item->getTask()->setState(Task::TERMINATING);
  }
}


QString
TaskProgressWidget::custommatTime(double sec)
{
  int s = int(sec)%60;
  int m = int(sec)/60;
  int h = int(m)/60; m = m % 60;
  int d = int(h)/24; h = h % 24;

  if (d > 0)
    return QString("%1d %2h %3m %4s").arg(d).arg(h).arg(m).arg(s);

  if (h > 0)
    return QString("%1h %2m %3s").arg(h).arg(m).arg(s);

  if (m > 0)
    return QString("%1m %2s").arg(m).arg(s);

  return QString("%1s").arg(s);
}
