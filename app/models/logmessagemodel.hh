#ifndef __INA_APP_LOGMESSAGEMODEL_HH__
#define __INA_APP_LOGMESSAGEMODEL_HH__

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QFile>
#include <utils/logger.hh>



/** Wraps a @c Fluc::Utils::Message to be passed around with Qt signals. */
class MessageWrapper : public QObject
{
  Q_OBJECT

public:
  /** Constructor. */
  MessageWrapper(const iNA::Utils::Message &message, QObject *parent=0);

  /** Returns the message text as a QString. */
  QString getText() const;
  /** Returns the source file location. */
  QString getFile() const;
  /** Returns the line of the source file locaiton. */
  size_t getLine() const;
  /** Returns the time-stamp. */
  QDateTime getTime() const;
  /** Retunrs the log level. */
  iNA::Utils::Message::Level getLevel() const;
  /** Returns the level name. */
  QString getLevelName() const;
  /** Returns the message instance. */
  const iNA::Utils::Message &message() const;

private:
  /** The message passed. */
  iNA::Utils::Message _message;
};



/** The handler that receives the messages from @c Fluc::Utils::Logger and emmits a
 * signal with that message. */
class LogWindowMessageHandler : public QObject, public iNA::Utils::MessageHandler
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit LogWindowMessageHandler();

  /** A callback that receives the log message. */
  virtual void handleMessage(const iNA::Utils::Message &message);

signals:
  /** Emmits the received log message. */
  void newMessage(MessageWrapper *message);
};



/** Collects all logmessages. */
class LogMessageModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  explicit LogMessageModel(QObject *parent = 0);
  
  virtual int rowCount(const QModelIndex &parent) const;
  virtual int columnCount(const QModelIndex &parent) const;
  virtual QVariant data(const QModelIndex &index, int role) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

  const iNA::Utils::Message &message(int row);

private slots:
  /** Will be called for each logged message. */
  void onNewMessage(MessageWrapper *message);

private:
  /** Holds all messages. */
  QList<iNA::Utils::Message> _messages; 
};



/** Wraps the @c LogMessageModel into a @c QSortFilterProxyModel to filter messages by
 * level. You do not need to instantiate the @c LogMessageModel explicitly and assign it as the
 * source model to this proxy. This is done in the constructor. */
class FilteredLogMessageModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:
  explicit FilteredLogMessageModel(QObject *parent = 0);
  void setFilterLevel(iNA::Utils::Message::Level level);
  bool saveLog(QFile &file);

protected:
  virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
  iNA::Utils::Message::Level _level;
  LogMessageModel *_model;
};

#endif // __INA_APP_LOGMESSAGEMODEL_HH__
