#ifndef __INA_UTILS_LOGGER_HH__
#define __INA_UTILS_LOGGER_HH__

#include <sstream>
#include <list>


namespace Fluc {
namespace Utils {


/**
 * A simple message class.
 */
class Message : public std::stringstream
{
public:
  /**
   * Lists all levels of messages.
   */
  typedef enum {
    DEBUG,  ///< Debug message.
    INFO,   ///< Status updates.
    WARN,   ///< Warnings.
    ERROR   ///< Errors.
  } Level;


protected:
  Level _level;
  size_t _line;
  std::string _file;

public:
  Message(Level level);
  Message(Level level, const std::string &file, size_t line);
  Message(Level level, const std::string &file, size_t line, const std::string &message);
  Message(const Message &other);

  std::string getText() const;
  const std::string &getFileName() const;
  size_t getLineNo() const;
  Level getLevel() const;

  Message &operator =(const Message &other);
};



/**
 * Message handler interface.
 */
class MessageHandler {
public:
  virtual void handleMessage(const Message &message) = 0;
};


/**
 * This class implements a simple message logger.
 */
class Logger
{
public:
  /** Sends a message. */
  void log(const Message &message);
  /** Adds a message handler to the logger.
   * @note The logger takes the ownership of the handler.
   */
  void addHandler(MessageHandler *handler);

public:
  /** Destructor. Also frees all message handlers. */
  ~Logger();
  /** Retruns the global logger instance. */
  static Logger &get();
  /** Destroys the global logger instance. */
  static void shutdown();
  /** Directly sends a debug message. */
  static void debug(const std::string &message);
  /** Directly sends a info message. */
  static void info(const std::string &message);
  /** Directly sends a warning message. */
  static void warn(const std::string &message);
  /** Directly sends a error message. */
  static void error(const std::string &messae);

protected:
  /** Hidden constructor. Use @c Logger::get() to get the global instance. */
  Logger();

private:
  /** Holds the list of message handlers. */
  std::list<MessageHandler *> _handlers;
  /** Singleton instance of the logger. */
  static Logger *_singleton_instance;
};



/** Simple macro to create a log-message at source location. */
#define LOG_MESSAGE(level) Fluc::Utils::Message(level, __FILE__, __LINE__)



/**
 * The default handler class, prints messages into a text stream (i.e. stdout).
 */
class TextMessageHandler : public MessageHandler
{
protected:
  Message::Level _level;
  std::ostream &_stream;

public:
  TextMessageHandler(std::ostream &stream, Message::Level level=Message::INFO);

  virtual void handleMessage(const Message &message);
};


}
}

#endif // __INA_UTILS_LOGGER_HH__
