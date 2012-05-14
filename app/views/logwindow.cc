#include "logwindow.hh"
#include <QDateTime>
#include <QHeaderView>


MessageWrapper::MessageWrapper(const Fluc::Utils::Message &message, QObject *parent)
  : QObject(parent), _message(message)
{
  // Pass...
}

QString
MessageWrapper::getText() const
{
  return QString::fromStdString(_message.getText());
}

QString
MessageWrapper::getFile() const
{
  return QString::fromStdString(_message.getFileName());
}

size_t
MessageWrapper::getLine() const
{
  return _message.getLevel();
}

Fluc::Utils::Message::Level
MessageWrapper::getLevel() const
{
  return _message.getLevel();
}

QDateTime
MessageWrapper::getTime() const
{
  return QDateTime::fromTime_t(_message.getTime());
}

QString
MessageWrapper::getLevelName() const
{
  switch (_message.getLevel()) {
  case Fluc::Utils::Message::DEBUG:
    return "debug";
  case Fluc::Utils::Message::INFO:
    return "info";
  case Fluc::Utils::Message::WARN:
    return "warn";
  case Fluc::Utils::Message::ERROR:
    return "error";
  }
  return "";
}


LogWindowMessageHandler::LogWindowMessageHandler()
  : QObject(0)
{
  // Pass...
}

void
LogWindowMessageHandler::handleMessage(const Fluc::Utils::Message &message)
{
  emit newMessage(new MessageWrapper(message));
}



LogWindow::LogWindow(QWidget *parent) :
    QTableWidget(parent)
{
  this->setWindowTitle("log");
  this->setColumnCount(2);
  QStringList labels; labels.append("time"); labels.append("message");
  this->setHorizontalHeaderLabels(labels);
  this->horizontalHeader()->setStretchLastSection(true);

  this->setShowGrid(false); this->verticalHeader()->setVisible(false);
  this->setMinimumSize(640, 100);

  // Install message handler:
  LogWindowMessageHandler *handler = new LogWindowMessageHandler();
  QObject::connect(handler, SIGNAL(newMessage(MessageWrapper*)), SLOT(onMessage(MessageWrapper*)));
  Fluc::Utils::Logger::get().addHandler(handler);
}


void
LogWindow::onMessage(MessageWrapper *message)
{
  size_t i=this->rowCount(); this->setRowCount(i+1);
  QTableWidgetItem *time_item  = new QTableWidgetItem(message->getTime().time().toString());
  QTableWidgetItem *text_item  = new QTableWidgetItem(message->getText());
  this->setItem(i, 0, time_item); this->setItem(i, 1, text_item);
  time_item->setFlags(Qt::NoItemFlags); text_item->setFlags(Qt::NoItemFlags);

  switch(message->getLevel()) {
  case Fluc::Utils::Message::DEBUG:
    time_item->setTextColor(QColor::fromRgb(0xa0, 0xa0, 0xa0));
    text_item->setTextColor(QColor::fromRgb(0xa0, 0xa0, 0xa0));
    break;

  case Fluc::Utils::Message::INFO:
    time_item->setBackgroundColor(QColor::fromRgb(0xff, 0xff, 0xff));
    text_item->setBackgroundColor(QColor::fromRgb(0xff, 0xff, 0xff));
    break;

  case Fluc::Utils::Message::WARN:
    time_item->setBackgroundColor(QColor::fromRgb(0xff, 0xf0, 0xc0));
    text_item->setBackgroundColor(QColor::fromRgb(0xff, 0xf0, 0xc0));
    break;

  case Fluc::Utils::Message::ERROR:
    time_item->setBackgroundColor(QColor::fromRgb(0xff, 0xc0, 0xc0));
    text_item->setBackgroundColor(QColor::fromRgb(0xff, 0xc0, 0xc0));
    break;
  }

  this->scrollToItem(time_item, QAbstractItemView::PositionAtBottom);
}
