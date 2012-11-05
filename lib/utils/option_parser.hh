#ifndef __FLUC_UTILS_OPTION_PARSER_HH__
#define __FLUC_UTILS_OPTION_PARSER_HH__

#include <list>
#include <string>
#include <set>
#include <map>


namespace iNA{
namespace Utils {
namespace Opt {


// forward declaration:
class Parser;

/**
 * Interface class for all option rules.
 * @ingroup optionparser
 */
class RuleInterface
{
public:
  virtual ~RuleInterface();

  /**
   * This method will be implemented by all option rules to perform option parsing.
   *
   * @param argv Array of arguments provided from main().
   * @param argc Number of arguments given by main().
   * @param idx Reference to the index of the argument being processed currently.
   * @param parser Reference to the OptionParser instance to store values of options and
   *   arugmuemnts.
   */
  virtual bool operator()(const char *argv[], int argc, size_t &idx, Parser &parser) = 0;

  /** This mehtod will be implemented to generate a string representation of the option
   * rules. */
  virtual std::string dump() = 0;

  /** Constructs a list of alternatives: @c OneOfRule. */
  RuleInterface &operator| (RuleInterface &other);

  /** Constructs a @c ListRule option-rule. */
  RuleInterface &operator, (RuleInterface &other);
};



/** A rule representing an option or flag.
 * @ingroup optionparser
 */
class OptionRule: public RuleInterface
{
protected:
  /** The short name (single charater) of the option. */
  char short_name;

  /** The long name for this option. */
  std::string long_name;

  /** \todo Is this member needed? I thougth all values are stored in the Parser object? */
  std::string value;

  /** If this is true the option does not accept a value, it is a flag. */
  bool is_flag;


protected:
  /**
   * Constructor.
   *
   * @param long_name Specifies the long name for the option.
   * @param is_flag Specifies if the option is a flag and do not take a value.
   * @param short_name Specifies the short name for the option. By default it is 0 this
   *    means there is no short name.
   */
  OptionRule(std::string long_name, bool is_flag = false, char short_name=0);


public:
  /** Performs the actual option parsing. */
  virtual bool operator()(const char *argv[], int argc, size_t &idx, Parser &prs);

  /** Generates a string representation for the option rule. */
  virtual std::string dump();

  friend class Parser;
};



/** Consumes exactly on option rule from the owned set of options.
 * This class can be used to express conflicting options.
 * @ingroup optionparser
 */
class OneOfRule: public RuleInterface
{
protected:
  /** Holds a list of the options that one has to match. */
  std::list<RuleInterface *> options;


protected:
  /** \todo Is this needed? */
  OneOfRule();

  /** The constructor takes a list of option rules. The ownership of these rules is
   * tranferred to this object and they are destroyed if this object is destroyed. */
  OneOfRule(std::list<RuleInterface *> &options);

  /** The constructor takes the number of optional rules and a list of @c OptionRuleInterface
   * instances. */
  OneOfRule(size_t n, ...);


public:
  /** Destructor. Destroies also the encapsulated option rules. */
  virtual ~OneOfRule();

  /** Tries to parse the option rule, returns true on success. */
  virtual bool operator()(const char *argv[], int argc, size_t &idx, Parser &parser);

  /** Dumps the rule expression. */
  virtual std::string dump();

  friend class Parser;
  friend class RuleInterface;
};



/**
 * Consumes any number of the enclosed option rules to be applied.
 * @ingroup optionparser
 */
class ZeroOrMoreRule: public RuleInterface
{
protected:
  /** Holds the encapsulated option rule. */
  RuleInterface *rule;


protected:
  /** Constructor. The ownership of the given rule is transferred to this object and will be
   * destroyed if this object gets destroyed. */
  ZeroOrMoreRule(RuleInterface *rule);

public:
  /** Destructor. Also destroies the encapsulated option rule. */
  virtual ~ZeroOrMoreRule();

  virtual bool operator()(const char *argv[], int argc, size_t &idx, Parser &parser);

  virtual std::string dump();

  friend class Parser;
};



/**
 * Represents a value of an option.
 * @ingroup optionparser
 */
class ValueRule: public RuleInterface
{
protected:
  /**
   * Will hold the value of the argument.
   */
  std::string value;

protected:
  ValueRule() { /* pass... */ }

public:
  virtual bool operator()(const char *argv[], int argc, size_t &idx, Parser &parser);

  virtual std::string dump();

  friend class Parser;
};



/**
 * Represents a list of option rules that all have to match the input in the given order.
 * @ingroup optionparser
 */
class ListRule: public RuleInterface
{
protected:
  /**
   * Holds the list of options.
   */
  std::list<RuleInterface *> options;


protected:
  /**
   * Constructor. The ownership of the options given is transferred to this object. They
   * will be destroyed if this object is destroyed.
   */
  ListRule(std::list<RuleInterface *> &opts);

  /**
   * Hidden constructor with variable arguments.
   */
  ListRule(size_t n, ...);


public:
  /**
   * Destructor. Also destroies the encapsulated options.
   */
  virtual ~ListRule();


  virtual bool operator()(const char *argv[], int argc, size_t &idx, Parser &parser);

  virtual std::string dump();

  friend class Parser;
  friend class RuleInterface;
};



/**
 * An optional rule.
 * @ingroup optionparser
 */
class OptionalRule: public RuleInterface
{
protected:
  /**
   * The optional option.
   */
  RuleInterface *rule;


protected:
  /**
   * The constructor will take the ownership of the given option.
   */
  OptionalRule(RuleInterface *rule);

public:
  /**
   * The destructor also destroies the option held in "rule".
   */
  virtual ~OptionalRule();

  virtual bool operator()(const char *argv[], int argc, size_t &idx, Parser &parser);

  virtual std::string dump();

  friend class Parser;
};



/**
 * The parser class encapsulating all option rules.
 *
 * The following example shows how to use the option parser for a simple case:
 *
 * \code
 * int main(char *argv[], int argc)
 * {
 *   Parser parser(
 *      Parser::opt(Parser::Flag("help") | Parser::Flag("version")) );
 *
 *   if (! parser(argv, argc)) {
 *     std::cerr << parser.format_help(argv[0]);
 *     return -1;
 *   }
 *
 *   return 0;
 * }
 * \endcode
 *
 * In this example, there are two flags, that exclude eachother but they are optional.
 * This means you could call with --help, with --version or without any flag. But to call
 * the program with --help and --flag would be invalid.
 *
 * The example above can be extended with an mandory argument:
 * \code
 * Parser parser(
 *    (Parser::opt(Parser::Flag("help") | Parser::Flag("version")), Parser::Value()) );
 * \endcode
 *
 * @ingroup optionparser
 */
class Parser
{
protected:
  /**
   * Holds the start-rule of the option grammar.
   */
  RuleInterface *rule;

  /**
   * A set of all long option names of present flags.
   */
  std::set<std::string> present_flags;

  /**
   * A map "long name" -> "values" of all present options.
   */
  std::map<std::string, std::list<std::string> > present_options;

  /**
   * A list holding all arguments present.
   */
  std::list<std::string> given_values;


public:
  /**
   * The constructor takes the ownership of the start rule given.
   */
  Parser(RuleInterface &rule);

  /**
   * Destroies also the start-rule of the option grammar.
   */
  ~Parser();

  /**
   * Actually initializing the option parsing.
   */
  bool parse(const char *argv[], int argc);

  /**
   * Formats a help/usage string.
   */
  std::string format_help(std::string prg_name);

  /**
   * Sets a flag specified by its long name to be present. This method is called by the
   * option rules.
   */
  void set_flag(std::string name);

  /**
   * Returns true if the flag has been found while parsing.
   */
  bool has_flag(std::string name);

  /**
   * Sets the value of the option. This method is called by the option rule.
   */
  void set_option(std::string name, std::string value);

  /**
   * Returns true if the option was present.
   */
  bool has_option(std::string name);

  /**
   * Returns the value of the option.
   */
  const std::list<std::string> &get_option(std::string name);

  /**
   * Adds a argument.
   */
  void add_value(std::string value);

  /**
   * Returns the list of arguments found while parsing.
   */
  const std::list<std::string> &get_values();


public:
  /**
   * Constructs a @c OptionRule as a flag.
   */
  static RuleInterface &Flag(const std::string &name, char short_name=0) {
    return *(new OptionRule(name, true, short_name));
  }

  /**
   * Constructs a @c OptionRule as an option.
   */
  static RuleInterface &Option(const std::string &name, char short_name=0) {
    return *(new OptionRule(name, false, short_name));
  }

  /**
   * Constructs a @c ValueRule.
   */
  static RuleInterface &Value() {
    return *(new ValueRule());
  }

  /**
   * Constructs a @c OptionalRule.
   */
  static RuleInterface &opt(RuleInterface &rule) {
    return *(new OptionalRule(&rule));
  }

  /**
   * Constructs a @c ZeroOrMoreRule.
   */
  static RuleInterface &zeroOrMore(RuleInterface &rule) {
    return *(new ZeroOrMoreRule(&rule));
  }
};


}
}
}

#endif
