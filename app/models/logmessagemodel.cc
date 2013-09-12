#include "logmessagemodel.hh"
#include <QDateTime>
#include <QTextStream>
#include <QBrush>
#include <QFileInfo>


MessageWrapper::MessageWrapper(const iNA::Utils::Message &message, QObject *parent)
  : QObject(parent), _message(message)
{
  // Pass...
}

QString
MessageWrapper::getText() const {
  return QString::fromStdString(_message.getText());
}

QString
MessageWrapper::getFile() const {
  return QString::fromStdString(_message.getFileName());
}

size_t
MessageWrapper::getLine() const {
  return _message.getLevel();
}

iNA::Utils::Message::Level
MessageWrapper::getLevel() const {
  return _message.getLevel();
}

QDateTime
MessageWrapper::getTime() const {
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

const iNA::Utils::Message &
MessageWrapper::message() const {
  return _message;
}



LogWindowMessageHandler::LogWindowMessageHandler()
  : QObject(0)
{
  // Pass...
}

void
LogWindowMessageHandler::handleMessage(const iNA::Utils::Message &message) {
  MessageWrapper *wrapper = new MessageWrapper(message);
  emit newMessage(wrapper);
  wrapper->deleteLater();
}




/* ******************************************************************************************** *
 * Receives all messages.
 * ******************************************************************************************** */
LogMessageModel::LogMessageModel(QObject *parent)
  : QAbstractTableModel(parent), _messages()
{
  LogWindowMessageHandler *handler = new LogWindowMessageHandler();
  iNA::Utils::Logger::get().addHandler(handler);
  QObject::connect(handler, SIGNAL(newMessage(MessageWrapper*)),
                   this, SLOT(onNewMessage(MessageWrapper*)));
}

int
LogMessageModel::rowCount(const QModelIndex &parent) const {
  return _messages.size();
}

int
LogMessageModel::columnCount(const QModelIndex &parent) const {
  return 3;
}

QVariant
LogMessageModel::data(const QModelIndex &index, int role) const {
  if (index.row() >= _messages.size()) { return QVariant(); }
  if (index.column() >= 4) { return QVariant(); }

  // Select color by level:
  if (role == Qt::ForegroundRole) {
    // Get level:
    iNA::Utils::Message::Level level = _messages.at(index.row()).getLevel();
    // Select color by level:
    QBrush brush(QColor(0x00, 0x00, 0x00));
    switch (level) {
    case iNA::Utils::Message::DEBUG:
      brush.setColor(QColor(0xa0, 0xa0, 0xa0)); break;
    case iNA::Utils::Message::INFO:
      brush.setColor(QColor(0x00, 0x00, 0x00)); break;
    case iNA::Utils::Message::WARN:
      brush.setColor(QColor(0x00, 0x00, 0xff)); break;
    case iNA::Utils::Message::ERROR:
      brush.setColor(QColor(0xff, 0x00, 0x00)); break;
    }
    return QVariant(brush);
  }

  if (role == Qt::DisplayRole) {
    if (0 == index.column()) {
      return QDateTime::fromTime_t(_messages.at(index.row()).getTime()).toString();
    } else if (1 == index.column()) {
      return QFileInfo(QString::fromStdString(_messages.at(index.row()).getFileName())).fileName();
    } else if (2 == index.column()) {
      return QString(_messages.at(index.row()).getText().c_str());
    }
  }

  return QVariant();
}


QVariant
LogMessageModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (Qt::Horizontal != orientation) { return QVariant(); }
  if (Qt::DisplayRole != role) { return QVariant(); }

  switch (section) {
  case 0: return tr("time");
  case 1: return tr("file");
  case 2: return tr("message");
  default: break;
  }

  return QVariant();
}


const iNA::Utils::Message &
LogMessageModel::message(int row) {
  return _messages.at(row);
}


void
LogMessageModel::onNewMessage(MessageWrapper *message) {
  int row_idx = rowCount(QModelIndex());
  beginInsertRows(QModelIndex(), row_idx, row_idx);
  _messages.append(message->message());
  endInsertRows();
}




/* ******************************************************************************************** *
 * Sorts and filters messages by level.
 * ******************************************************************************************** */
FilteredLogMessageModel::FilteredLogMessageModel(QObject *parent)
  : QSortFilterProxyModel(parent), _level(iNA::Utils::Message::DEBUG), _model(0)
{
  // Create message model and store as source model:
  _model = new LogMessageModel(this);
  setSourceModel(_model);
}

void
FilteredLogMessageModel::setFilterLevel(iNA::Utils::Message::Level level) {
  _level = level;
  invalidateFilter();
}

bool
FilteredLogMessageModel::saveLog(QFile &file) {
  QTextStream stream(&file);
  for (int i=0; i<rowCount(); i++) {
    QModelIndex idx(index(i, 0));
    QString date = data(index(i,0), Qt::DisplayRole).toString();
    QString message = data(index(i, 1), Qt::DisplayRole).toString();
    stream << date << ": " << message << "\n";
  }
  return true;
}

bool
FilteredLogMessageModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
  if (sourceRow >= _model->rowCount(QModelIndex())) { return false; }
  // Filter by level:
  return _model->message(sourceRow).getLevel() >= _level;
}


