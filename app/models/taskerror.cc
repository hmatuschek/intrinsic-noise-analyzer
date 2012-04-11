#include "taskerror.hh"

TaskError::TaskError(QObject *parent) :
  QObject(parent)
{
  // Pass...
}


TaskError::TaskError(const QString &title, const QString &details, QObject *parent)
  : QObject(parent), title(title), details(details)
{
  // Pass...
}


void
TaskError::setTitle(const QString &title)
{
  this->title = title;
}

void
TaskError::setDetails(const QString &details)
{
  this->details = details;
}


const QString &
TaskError::getTitle() const
{
  return this->title;
}


const QString &
TaskError::getDetails() const
{
  return this->details;
}
