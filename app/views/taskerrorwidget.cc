#include "taskerrorwidget.hh"
#include <QVBoxLayout>
#include "../models/application.hh"


TaskErrorWidget::TaskErrorWidget(TaskItem *task, QWidget *parent) :
  QWidget(parent), task_wrapper(task)
{
  // Create widgets:
  this->title = new QLabel();
  this->title->setFont(Application::getApp()->getH1Font());
  QPalette title_palette = this->title->palette();
  title_palette.setColor(this->title->customegroundRole(), Qt::red);
  this->title->setPalette(title_palette);
  this->title->setAlignment(Qt::AlignHCenter);
  this->title->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  this->details = new QLabel();
  this->details->setAlignment(Qt::AlignLeft);
  this->details->setWordWrap(true);
  this->details->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Layout widgets
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(this->title);
  layout->addSpacing(20);
  layout->addWidget(this->details);
  layout->addStretch(0);
  this->setLayout(layout);

  // Connect signals:
  QObject::connect(this->task_wrapper->getTask(), SIGNAL(stateChanged()), this, SLOT(taskStateChanged()));

  // If task is already in ERROR state:
  if (Task::ERROR == task_wrapper->getTask()->getState())
  {
    this->setError(task_wrapper->getTask()->getErrorMessage());
  }
}


void
TaskErrorWidget::setError(const TaskError &error)
{
  this->title->setText(error.getTitle());
  this->details->setText(error.getDetails());
}


void
TaskErrorWidget::taskStateChanged()
{
  if (Task::ERROR == this->task_wrapper->getTask()->getState())
  {
    this->setError(this->task_wrapper->getTask()->getErrorMessage());
  }
}


