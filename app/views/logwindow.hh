#ifndef __INA_APP_VIEWS_LOGWINDOW_HH__
#define __INA_APP_VIEWS_LOGWINDOW_HH__

#include <QTableWidget>
#include <QString>

#include "utils/logger.hh"


/**
 * Wraps a @c Fluc::Utils::Message to be passed around with Qt signals.
 */
class MessageWrapper : public QObject
{
  Q_OBJECT

private:
  Fluc::Utils::Message _message;

public:
  MessageWrapper(const Fluc::Utils::Message &message, QObject *parent=0);

  QString getText() const;
  QString getFile() const;
  size_t getLine() const;
  QDateTime getTime() const;
  Fluc::Utils::Message::Level getLevel() const;
  QString getLevelName() const;
};


/**
 * The handler that receives the messages from @c Fluc::Utils::Logger.
 */
class LogWindowMessageHandler : public QObject, public Fluc::Utils::MessageHandler
{
  Q_OBJECT

public:
  explicit LogWindowMessageHandler();

  virtual void handleMessage(const Fluc::Utils::Message &message);

signals:
  void newMessage(MessageWrapper *message);
};


/**
 * A simple log-window widget.
 */
class LogWindow : public QTableWidget
{
  Q_OBJECT

public:
  explicit LogWindow(QWidget *parent = 0);

private slots:
  void onMessage(MessageWrapper *message);
};

#endif // LOGWINDOW_HH
