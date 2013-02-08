#ifndef __INA_APP_VIEWS_LOGWINDOW_HH__
#define __INA_APP_VIEWS_LOGWINDOW_HH__

#include <QTableWidget>
#include <QString>

#include "utils/logger.hh"


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


/** A simple log-table widget. */
class LogTable : public QTableWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit LogTable(QWidget *parent = 0);

public slots:
  /** Saves the log messages into the given file.
   * Returns true on success. */
  bool saveLog(const QString &filename);

private slots:
  /** Receives messages from LogWindowMessageHandler. */
  void onMessage(MessageWrapper *message);
};


/** The log window. */
class LogWindow : public QWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  LogWindow(QWidget *parent=0);

private slots:
  void onSaveLog();

private:
  /** Holds the logtable instance. */
  LogTable *_logtable;
};

#endif
