#include "logwindow.hh"
#include <QDateTime>
#include <QHeaderView>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>


MessageWrapper::MessageWrapper(const iNA::Utils::Message &message, QObject *parent)
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

iNA::Utils::Message::Level
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
  case iNA::Utils::Message::DEBUG:
    return "debug";
  case iNA::Utils::Message::INFO:
    return "info";
  case iNA::Utils::Message::WARN:
    return "warn";
  case iNA::Utils::Message::ERROR:
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
LogWindowMessageHandler::handleMessage(const iNA::Utils::Message &message)
{
  emit newMessage(new MessageWrapper(message));
}



LogTable::LogTable(QWidget *parent) :
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
  iNA::Utils::Logger::get().addHandler(handler);
}


bool
LogTable::saveLog(const QString &filename)
{
  QFile file(filename);
  if (! file.open(QFile::WriteOnly)) {
    QMessageBox::critical(
          0, tr("Can not create/open file"),
          tr("Can not write log to file %1: Can not be opened for writing.").arg(filename));
    return false;
  }

  {
    QTextStream stream(&file);
    for (int i=0; i<this->rowCount(); i++) {
      stream << this->item(i,0)->text()
             << ": " << this->item(i,1)->text() << "\n";
    }
  }
  file.close();
  return true;
}


void
LogTable::onMessage(MessageWrapper *message)
{
  size_t i=this->rowCount(); this->setRowCount(i+1);
  QTableWidgetItem *time_item  = new QTableWidgetItem(message->getTime().time().toString());
  QTableWidgetItem *text_item  = new QTableWidgetItem(message->getText());
  this->setItem(i, 0, time_item); this->setItem(i, 1, text_item);
  time_item->setFlags(Qt::NoItemFlags); text_item->setFlags(Qt::NoItemFlags);

  switch(message->getLevel()) {
  case iNA::Utils::Message::DEBUG:
    time_item->setTextColor(QColor::fromRgb(0xa0, 0xa0, 0xa0));
    text_item->setTextColor(QColor::fromRgb(0xa0, 0xa0, 0xa0));
    break;

  case iNA::Utils::Message::INFO:
    time_item->setBackgroundColor(QColor::fromRgb(0xff, 0xff, 0xff));
    text_item->setBackgroundColor(QColor::fromRgb(0xff, 0xff, 0xff));
    break;

  case iNA::Utils::Message::WARN:
    time_item->setBackgroundColor(QColor::fromRgb(0xff, 0xf0, 0xc0));
    text_item->setBackgroundColor(QColor::fromRgb(0xff, 0xf0, 0xc0));
    break;

  case iNA::Utils::Message::ERROR:
    time_item->setBackgroundColor(QColor::fromRgb(0xff, 0xc0, 0xc0));
    text_item->setBackgroundColor(QColor::fromRgb(0xff, 0xc0, 0xc0));
    break;
  }

  this->scrollToItem(time_item, QAbstractItemView::PositionAtBottom);
}



LogWindow::LogWindow(QWidget *parent)
  : QWidget(parent)
{
  _logtable = new LogTable();
  QPushButton *button = new QPushButton(tr("save"));

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_logtable);
  layout->addWidget(button,0, Qt::AlignRight);
  setLayout(layout);

  QObject::connect(button, SIGNAL(clicked()), this, SLOT(onSaveLog()));
}

void
LogWindow::onSaveLog() {
  QString filename = QFileDialog::getSaveFileName(
        0, tr("Save log to"), "", tr("Text files (*.txt)"));
  if (0 == filename.size()) { return; }
  _logtable->saveLog(filename);
}
