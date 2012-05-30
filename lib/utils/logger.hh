#ifndef __INA_UTILS_LOGGER_HH__
#define __INA_UTILS_LOGGER_HH__

#include <sstream>
#include <list>
#include <ctime>


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
  /** Holds the log level of the message. */
  Level _level;
  /** Optionally hods the code-line of origin of the message. */
  size_t _line;
  /** Optionally hods the code-file of origin of the message. */
  std::string _file;
  /** Holds the time-stamp of the message. */
  time_t _time;

public:
  /** Constructor with level. */
  Message(Level level);
  /** Constructor with level and origin. */
  Message(Level level, const std::string &file, size_t line);
  /** Constructor with level, origin and message. */
  Message(Level level, const std::string &file, size_t line, const std::string &message);
  /** Copy constructor. */
  Message(const Message &other);

  /** Returns the message text. */
  std::string getText() const;
  /** Returns the filename of the origin, or an empty string if not set. */
  const std::string &getFileName() const;
  /** Returns the line number of the origin, or 0 if not set. */
  size_t getLineNo() const;
  /** Returns the log level of the message. */
  Level getLevel() const;
  /** Returns the time stamp. */
  const time_t &getTime() const;

  /** Assignment operator, copies the RHS message. */
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
  /** Level of filter. */
  Message::Level _level;
  /** The ostream to send text to. */
  std::ostream &_stream;

public:
  /** Constructor. */
  TextMessageHandler(std::ostream &stream, Message::Level level=Message::INFO);
  /** Handles a log message. */
  virtual void handleMessage(const Message &message);
};


}
}

#endif // __INA_UTILS_LOGGER_HH__
