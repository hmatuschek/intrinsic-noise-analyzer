#ifndef __FLUC_OPTION_PARSER_HH__
#define __FLUC_OPTION_PARSER_HH__

#include <list>
#include <string>
#include <set>
#include <map>


namespace Fluc{


// forward declaration:
class OptionParser;

/**
 * Interface class for all option rules.
 *
 * @ingroup utils
 */
class IOptionRule
{
public:
  virtual ~IOptionRule();

  /**
   * This method will be implemented by all option rules to perform option parsing.
   *
   * @param argv Array of arguments provided from main().
   * @param argc Number of arguments given by main().
   * @param idx Reference to the index of the argument being processed currently.
   * @param parser Reference to the OptionParser instance to store values of options and
   *   arugmuemnts.
   */
  virtual bool operator()(char *argv[], int argc, size_t &idx, OptionParser &parser) = 0;

  /**
   * This mehtod will be implemented to generate a string representation of the option
   * rules.
   */
  virtual std::string dump() = 0;
};



/**
 * A rule representing an option or flag.
 *
 * @ingroup utils
 */
class OptionRule: public IOptionRule
{
protected:
  /**
   * The short name (single charater) of the option.
   */
  char short_name;

  /**
   * The long name for this option.
   */
  std::string long_name;

  /**
   * \todo Is this member needed? I thougth all values are stored in the Parser object?
    */
  std::string value;

  /**
   * If this is true the option does not accept a value, it is a flag.
   */
  bool is_flag;


public:
  /**
   * Constructor.
   *
   * @param long_name Specifies the long name for the option.
   * @param is_flag Specifies if the option is a flag and do not take a value.
   * @param short_name Specifies the short name for the option. By default it is 0 this
   *    means there is no short name.
   */
  OptionRule(std::string long_name, bool is_flag = false, char short_name=0);

  /**
   * Performs the actual option parsing.
   */
  virtual bool operator()(char *argv[], int argc, size_t &idx, OptionParser &prs);

  /**
   * Generates a string representation for the option rule.
   */
  virtual std::string dump();
};



/**
 * Consumes exactly on option rule from the owned set of options.
 * This class can be used to express conflicting options.
 *
 * @ingroup utils
 */
class OptionRuleOneOf: public IOptionRule
{
protected:
  /**
   * Holds a list of the options that one has to match.
   */
  std::list<IOptionRule *> options;


public:
  /**
   * \todo Is this needed.
   */
  OptionRuleOneOf();

  /**
   * The constructor takes a list of option rules. The ownership of these rules is
   * tranferred to this object and they are destroyed if this object is destroyed.
   */
  OptionRuleOneOf(std::list<IOptionRule *> &options);

  /**
   * Destructor. Destroies also the encapsulated option rules.
   */
  virtual ~OptionRuleOneOf();

  virtual bool operator()(char *argv[], int argc, size_t &idx, OptionParser &parser);

  virtual std::string dump();
};



/**
 * Consumes any number of the enclosed option rules to be applied.
 *
 * @ingroup utils
 */
class OptionRuleZeroOrMore: public IOptionRule
{
protected:
  /**
   * Holds the encapsulated option rule.
   */
  IOptionRule *rule;


public:
  /**
   * Constructor. The ownership of the given rule is transferred to this object and will be
   * destroyed if this object gets destroyed.
   */
  OptionRuleZeroOrMore(IOptionRule *rule);

  /**
   * Destructor. Also destroies the encapsulated option rule.
   */
  virtual ~OptionRuleZeroOrMore();

  virtual bool operator()(char *argv[], int argc, size_t &idx, OptionParser &parser);

  virtual std::string dump();
};



/**
 * Represents a value of an option.
 *
 * @ingroup utils
 */
class OptionRuleValue: public IOptionRule
{
protected:
  /**
   * Will hold the value of the argument.
   */
  std::string value;


public:
  virtual bool operator()(char *argv[], int argc, size_t &idx, OptionParser &parser);

  virtual std::string dump();
};



/**
 * Represents a list of option rules that all have to match the input in the given order.
 *
 * @ingroup utils
 */
class OptionRuleList: public IOptionRule
{
protected:
  /**
   * Holds the list of options.
   */
  std::list<IOptionRule *> options;


public:
  /**
   * Constructor. The ownership of the options given is transferred to this object. They
   * will be destroyed if this object is destroyed.
   */
  OptionRuleList(std::list<IOptionRule *> &opts);

  /**
   * Destructor. Also destroies the encapsulated options.
   */
  virtual ~OptionRuleList();


  virtual bool operator()(char *argv[], int argc, size_t &idx, OptionParser &parser);

  virtual std::string dump();
};



/**
 * An optional rule.
 *
 * @ingroup utils
 */
class OptionRuleOptional: public IOptionRule
{
protected:
  /**
   * The optional option.
   */
  IOptionRule *rule;


public:
  /**
   * The constructor will take the ownership of the given option.
   */
  OptionRuleOptional(IOptionRule *rule);

  /**
   * The destructor also destroies the option held in "rule".
   */
  virtual ~OptionRuleOptional();

  virtual bool operator()(char *argv[], int argc, size_t &idx, OptionParser &parser);

  virtual std::string dump();
};



/**
 * The parser class encapsulating all option rules.
 *
 * @ingroup utils
 */
class OptionParser
{
protected:
  /**
   * Holds the start-rule of the option grammar.
   */
  IOptionRule *rule;

  /**
   * A set of all long option names of present flags.
   */
  std::set<std::string> present_flags;

  /**
   * A map "long name" -> "value" of all present options.
   */
  std::map<std::string, std::string> present_options;

  /**
   * A list holding all arguments present.
   */
  std::list<std::string> given_values;


public:
  /**
   * The constructor takes the ownership of the start rule given.
   */
  OptionParser(IOptionRule *rule);

  /**
   * Destroies also the start-rule of the option grammar.
   */
  ~OptionParser();


  /**
   * Actually initializing the option parsing.
   */
  bool parse(char *argv[], int argc);

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
  std::string get_option(std::string name);

  /**
   * Adds a argument.
   */
  void add_value(std::string value);

  /**
   * Returns the list of arguments found while parsing.
   */
  std::list<std::string> get_values();
};


}

#endif
