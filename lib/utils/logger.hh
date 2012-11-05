/**
 * @defgroup logger
 * @ingroup utils
 *
 * This class collects some classes implementing a simple message logging interface. It allows to
 * define a process wide default logger and to receive messages send to that logger.
 */
#ifndef __INA_UTILS_LOGGER_HH__
#define __INA_UTILS_LOGGER_HH__

#include <sstream>
#include <list>
#include <ctime>


namespace iNA {
namespace Utils {


/** A simple message class.
 * This message class is used to hold log messages custom the @c Logger system. Each message must have
 * a level indicating the significance of the actual message. Usually a log message is created using
 * the @c LOG_MESSAGE macro, wich also sets the origin of the message.
 *
 * \code{.cpp}
 * using namespace iNA::Utils;
 *
 * Message message = LOG_MESSAGE(Message::DEBUG);
 * message << "A debug message...";
 * Logger::get().log(message);
 * \endcode
 *
 * With @c Logger::get() you obtain the global default logger instance, which is used to handle
 * the log message here.
 * @ingroup logger
 */
class Message : public std::stringstream
{
public:
  /** Lists all levels of messages. */
  typedef enum {
    DEBUG,  ///< Debug message.
    INFO,   ///< Status updates.
    WARN,   ///< Warnings.
    ERROR   ///< Errors.
  } Level;


protected:
  /** Holds the log level of the message. */
  Level _level;
  /** Optionally holds the code-line of origin of the message. */
  size_t _line;
  /** Optionally holds the code-file of origin of the message. */
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



/** Message handler interface.
 * Each @c Logger instance can customward log messages to one or more message handlers. There is a
 * single pre-defined message handler @c TextMessageHandler, which custommats all message with a
 * a defined log level as text messages and customward this text into a defined text stream. It is
 * easy to meet the MessageHandler interface as just one method must be implemented to receive
 * messages.
 * @ingroup logger
 */
class MessageHandler {
public:
  /** Needs to be implemented by actual message handlers to process a message. */
  virtual void handleMessage(const Message &message) = 0;
};


/**
 * This class implements a simple message logger.
 *
 * @ingroup logger
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



/** Simple macro to create a log-message at source location.
 * @ingroup logger */
#define LOG_MESSAGE(level) iNA::Utils::Message(level, __FILE__, __LINE__)



/** The default handler class, prints messages into a text stream (i.e. stdout).
 * Call something like
 * \code{.cpp}
 * Logger::get().addHandler(new TextMessageHandler(std::cerr));
 * \endcode
 * to install the text message handler to the global default logger.
 * @ingroup logger
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
