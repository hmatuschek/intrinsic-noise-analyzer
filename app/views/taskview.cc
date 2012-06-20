#include "taskview.hh"
#include "../application.hh"
#include <QVBoxLayout>
#include <QPushButton>



/* ********************************************************************************************* *
 * Implementation of TaskView
 * ********************************************************************************************* */
TaskView::TaskView(TaskItem *task_wrapper, QWidget *parent)
  : QWidget(parent), task_item(task_wrapper)
{
  // Some basic layout stuff
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  // Construct widgets:
  this->title = new QLabel(task_wrapper->getTask()->getLabel());
  this->title->setFont(Application::getApp()->getH1Font());
  this->title->setAlignment(Qt::AlignRight);
  this->title->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

  this->stack = new QStackedWidget();
  this->stack->addWidget(new TaskProgressWidget(task_wrapper));
  this->stack->addWidget(new QLabel(tr("BUG: Specialized TaskView has not set a ResultWidget.")));
  this->stack->addWidget(new TaskErrorWidget(task_wrapper));
  this->stack->addWidget(new TaskTerminatingWidget());
  this->stack->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

  // construct layout:
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(this->title);
  layout->addWidget(this->stack);
  this->setLayout(layout);

  // Connect signals to task:
  QObject::connect(this->task_item->getTask(), SIGNAL(stateChanged()), this, SLOT(taskStateChanged()));
  QObject::connect(this->task_item->getTask(), SIGNAL(destroyed()), this, SLOT(taskDestroyed()));

  // Select visible widget by state:
  switch(task_wrapper->getTask()->getState())
  {
  case Task::INITIALIZED:
  case Task::RUNNING:
    this->stack->setCurrentIndex(0);
    break;

  case Task::DONE:
    this->stack->setCurrentIndex(1);
    break;

  case Task::ERROR:
    this->stack->setCurrentIndex(2);
    break;

  case Task::TERMINATING:
    this->stack->setCurrentIndex(3);
    break;
  }
}


TaskView::~TaskView()
{
  //std::cerr << "TaskView deleted." << std::endl;
  // Pass...
}


void
TaskView::taskStateChanged()
{
  // Select visible widget by state of task:
  switch (this->task_item->getTask()->getState())
  {
  case Task::INITIALIZED:
  case Task::RUNNING:
    this->stack->setCurrentIndex(0);
    break;

  case Task::DONE:
    this->stack->setCurrentIndex(1);
    break;

  case Task::ERROR:
    this->stack->setCurrentIndex(2);
    break;

  case Task::TERMINATING:
    this->stack->setCurrentIndex(2);
    break;
  }
}


void
TaskView::taskDestroyed()
{
  //std::cerr << "Task destroyed, destroy task view." << std::endl;

  // The view is invalid of the task gets destroyed:
  this->deleteLater();
}


void
TaskView::setProgressWidget(QWidget *widget)
{
  int index = this->stack->currentIndex();
  QWidget *old = this->stack->widget(0);

  this->stack->insertWidget(0, widget);
  this->stack->removeWidget(old);
  delete old;

  this->stack->setCurrentIndex(index);
}

void
TaskView::setResultWidget(QWidget *widget)
{
  int index = this->stack->currentIndex();
  QWidget *old = this->stack->widget(1);

  this->stack->insertWidget(1, widget);
  this->stack->removeWidget(old);
  delete old;

  this->stack->setCurrentIndex(index);
}

void
TaskView::setErrorWidget(QWidget *widget)
{
  int index = this->stack->currentIndex();
  QWidget *old = this->stack->widget(2);

  this->stack->insertWidget(2, widget);
  this->stack->removeWidget(old);
  delete old;

  this->stack->setCurrentIndex(index);
}



/* ********************************************************************************************* *
 * Implementation of TaskTerminatingWidget, just shows a message
 * ********************************************************************************************* */
TaskTerminatingWidget::TaskTerminatingWidget(QWidget *parent)
  : QLabel(tr("Waiting for thread to terminate..."), parent)
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
  this->time_elapsed = new QLabel(tr("Time elapsed: %1").arg(this->formatTime(dt)));
  this->time_elapsed->setAlignment(Qt::AlignCenter);
  this->time_elapsed->setTextFormat(Qt::LogText);
  this->time_elapsed->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  this->time_remain = new QLabel(tr("Time remaining: unknown"));
  this->time_remain->setAlignment(Qt::AlignCenter);
  this->time_remain->setTextFormat(Qt::LogText);
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
  this->time_elapsed->setText(tr("Time elapsed: %1").arg(this->formatTime(dt)));

  if (0 == dt || 0 == p)
  {
    this->time_remain->setText(tr("Time remaining: unknown"));
  }
  else
  {
    double time_r = (1. - p) * dt/p;
    this->time_remain->setText(tr("Time remaining: %1").arg(this->formatTime(time_r)));
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
TaskProgressWidget::formatTime(double sec)
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
