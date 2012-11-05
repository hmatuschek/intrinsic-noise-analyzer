#ifndef TASKERROR_HH
#define TASKERROR_HH

#include <QObject>

/**
 * This class collects some incustommation about an error occurred during an analysis task.
 *
 * Use @c TaskErrorWidget to display such an error message.
 *
 * @ingroup gui
 */
class TaskError : public QObject
{
  Q_OBJECT

protected:
  QString title;
  QString details;

public:
  explicit TaskError(QObject *parent = 0);
  explicit TaskError(const QString &title, const QString &details, QObject *parent = 0);

  void setTitle(const QString &title);
  void setDetails(const QString &details);
  const QString &getTitle() const;
  const QString &getDetails() const;
};


#endif // TASKERROR_HH
