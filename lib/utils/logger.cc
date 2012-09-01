#include "logger.hh"

using namespace iNA::Utils;


Message::Message(Level level)
  : std::stringstream(), _level(level), _line(0), _file(""), _time(time(0))
{
  // Pass...
}

Message::Message(Level level, const std::string &file, size_t line)
  : std::stringstream(), _level(level), _line(line), _file(file), _time(time(0))
{
  // Pass...
}

Message::Message(Level level, const std::string &file, size_t line, const std::string &message)
  : std::stringstream(), _level(level), _line(line), _file(file), _time(time(0))
{
  (*this) << message;
}

Message::Message(const Message &other)
  : std::stringstream(), _level(other._level), _line(other._line), _file(other._file),
    _time(other._time)
{
  (*this) << other.str();
}

std::string
Message::getText() const
{
  return this->str();
}

const std::string &
Message::getFileName() const
{
  return this->_file;
}

size_t
Message::getLineNo() const
{
  return this->_line;
}

Message::Level
Message::getLevel() const
{
  return this->_level;
}

const time_t &
Message::getTime() const
{
  return this->_time;
}

Message &
Message::operator =(const Message &other) {
  this->_level = other._level;
  this->_line = other._line;
  this->_file = other._file;
  this->_time = other._time;
  this->str(""); (*this) << other.str();

  return *this;
}



Logger *Logger::_singleton_instance = 0;

Logger::Logger()
  : _handlers()
{
  // pass...
}


Logger::~Logger()
{
  for(std::list<MessageHandler *>::iterator it = this->_handlers.begin();
      it != this->_handlers.end(); it++)
  {
    //delete *it;
  }
}


Logger &
Logger::get()
{
  if (0 == Logger::_singleton_instance) {
    Logger::_singleton_instance = new Logger();
  }

  return *Logger::_singleton_instance;
}

void
Logger::shutdown()
{
  if (0 != Logger::_singleton_instance) {
    delete Logger::_singleton_instance;
    Logger::_singleton_instance = 0;
  }
}

void
Logger::log(const Message &message)
{
  for (std::list<MessageHandler *>::iterator it = this->_handlers.begin();
       it != this->_handlers.end(); it++) {
    (*it)->handleMessage(message);
  }
}

void
Logger::addHandler(MessageHandler *handler)
{
  this->_handlers.push_back(handler);
}

void
Logger::debug(const std::string &message)
{
  get().log(Message(Message::DEBUG, "", 0, message));
}

void
Logger::info(const std::string &message)
{
  get().log(Message(Message::INFO, "", 0, message));
}

void
Logger::warn(const std::string &message)
{
  get().log(Message(Message::WARN, "", 0, message));
}

void
Logger::error(const std::string &message)
{
  get().log(Message(Message::ERROR, "", 0, message));
}


TextMessageHandler::TextMessageHandler(std::ostream &stream, Message::Level level)
  :  _level(level), _stream(stream)
{
  // Pass...
}

void
TextMessageHandler::handleMessage(const Message &message)
{
  // Filter messages by level
  if (message.getLevel() < this->_level)
    return;

  // Dump date-time
  char time_buffer[256];
  strftime(time_buffer, 256, "%Y-%m-%d %H:%M:%S", localtime(&message.getTime()));
  _stream << time_buffer << " ";

  // Dump level:
  switch (message.getLevel()) {
  case Message::DEBUG:
    _stream << "DEBUG: ";
    break;
  case Message::INFO:
    _stream << "INFO: ";
    break;
  case Message::WARN:
    _stream << "WARN: ";
    break;
  case Message::ERROR:
    _stream << "ERROR: ";
    break;
  }

  if (message.getFileName().size() > 0) {
    _stream << "In file " << message.getFileName() << " ";
    if (message.getLineNo() > 0) {
      _stream << "@ line " << message.getLineNo();
    }
    _stream << ":" << std::endl << "   ";
  }

  _stream << message.getText() << std::endl;
}
