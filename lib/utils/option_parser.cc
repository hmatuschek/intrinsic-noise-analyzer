#include "option_parser.hh"
#include <iostream>
#include <sstream>
#include <stdarg.h>


using namespace std;
using namespace iNA::Utils::Opt;


RuleInterface::~RuleInterface()
{
}


RuleInterface &
RuleInterface::operator |(RuleInterface &other)
{
  return *(new OneOfRule(2, this, &other));
}


RuleInterface &
RuleInterface::operator ,(RuleInterface &other)
{
  return *(new ListRule(2, this, &other));
}


OptionRule::OptionRule(std::string long_name, bool is_flag, char short_name)
  : short_name(short_name), long_name(long_name), is_flag(is_flag)
{
}


bool
OptionRule::operator()(const char *argv[], int argc, size_t &idx, Parser &parser)
{
  // If there is no option left on stack -> fail
  if(idx == (size_t)argc)
    return false;

  // copy option
  string str(argv[idx]);

  // if option string is shorter than 2 chars -> fail (this can not be an option)
  if(2 > str.size())
    return false;

  // if option string does not start with '-' -> fail
  if('-' != str[0])
    return false;

  // handle short option
  if(this->short_name == str[1])
  {
    if(2 != str.size())
      return false;

    if(not this->is_flag)
    {
      // consume option:
      idx++;

      // get value:
      // if there is no value
      if(idx == (size_t)argc)
        return false;

      this->value = argv[idx]; idx++;
      parser.set_option(this->long_name, this->value);
    }
    else
    {
      parser.set_flag(this->long_name);
    }

    return true;
  }

  // Handle long option:
  if('-' != str[1])
    return false;

  // handle flag;
  if(this->is_flag)
  {
    if(0 != str.substr(2).compare(this->long_name))
      return false;
    // consume option:
    idx++;
    parser.set_flag(this->long_name);
    return true;
  }

  // handle value option
  size_t name_end;
  // If VALUE is missing:
  if(str.npos == (name_end = str.find("=")))
    return false;

  // Check option name
  if(str.substr(2, name_end-2) != this->long_name)
    return false;

  // Set value
  this->value = str.substr(name_end+1);

  // Consume option...
  idx++;

  parser.set_option(this->long_name, this->value);

  return true;
}

string
OptionRule::dump()
{
  stringstream out;
  if(0 != this->short_name)
    {
      out << "(--" << this->long_name << "|-" << this->short_name << ")";
    }
  else
    {
      out << "--" << this->long_name;
    }

  if(not this->is_flag)
    {
      out << "=VALUE";
    }

  return out.str();
}


OneOfRule::OneOfRule()
{
}

OneOfRule::OneOfRule(std::list<RuleInterface *> &opts)
  : options(opts)
{
}


OneOfRule::OneOfRule(size_t n, ...)
{
  va_list args; va_start(args, n);
  for (size_t i=0; i<n; i++)
  {
    this->options.push_back(va_arg(args, RuleInterface *));
  }
  va_end(args);
}


OneOfRule::~OneOfRule()
{
  for(list<RuleInterface *>::iterator it=this->options.begin();
      it != this->options.end(); it++)
    {
      delete *it;
    }
}


bool
OneOfRule::operator()(const char *argv[], int argc, size_t &idx, Parser &parser)
{
  for(list<RuleInterface *>::iterator it = this->options.begin();
      it != this->options.end(); it++)
    {
      size_t tmp_idx = idx;
      if((**it)(argv, argc, tmp_idx, parser))
  {
    idx = tmp_idx;
    return true;
  }
    }

  return false;
}

std::string
OneOfRule::dump()
{
  stringstream out;
  list<RuleInterface *>::iterator it=this->options.begin();

  out << "(";
  if(0 != this->options.size())
    {
      for(size_t i=0; i<this->options.size()-1; i++)
  {
    out << (*it)->dump() << "|";
    it++;
  }
      out << (*it)->dump();
    }
  out << ")";

  return out.str();
}


bool
ValueRule::operator()(const char *argv[], int argc, size_t &idx, Parser &parser)
{
  if(idx == (size_t)argc)
    return false;

  this->value = argv[idx];
  // consume option
  idx++;
  parser.add_value(this->value);
  return true;
}

std::string
ValueRule::dump()
{
  return "ANYVALUE";
}


ListRule::ListRule(std::list<RuleInterface *> &opts)
  : options(opts)
{
}


ListRule::ListRule(size_t n, ...)
{
  va_list args; va_start(args, n);
  for (size_t i=0; i<n; i++)
  {
    this->options.push_back(va_arg(args, RuleInterface *));
  }
  va_end(args);
}


ListRule::~ListRule()
{
  for(list<RuleInterface *>::iterator it=this->options.begin();
      it != this->options.end(); it++)
    {
      delete *it;
    }
}

bool
ListRule::operator()(const char *argv[], int argc, size_t &idx, Parser &parser)
{
  if(idx == (size_t)argc)
    return false;

  for(list<RuleInterface *>::iterator it=this->options.begin();
      it != this->options.end(); it++)
    {
      if(not (**it)(argv, argc, idx, parser))
  return false;
    }

  return true;
}

std::string
ListRule::dump()
{
  stringstream out;
  list<RuleInterface *>::iterator it=this->options.begin();

  out << "(";
  if(0 != this->options.size())
    {
      for(size_t i=0; i<this->options.size()-1; i++)
  {
    out << (*it)->dump() << " ";
    it++;
  }
      out << (*it)->dump();
    }
  out << ")";

  return out.str();
}


OptionalRule::OptionalRule(RuleInterface *rule)
  : rule(rule)
{
}

OptionalRule::~OptionalRule()
{
  delete this->rule;
}

bool
OptionalRule::operator()(const char *argv[], int argc, size_t &idx, Parser &parser)
{
  if(idx == (size_t)argc)
    return true;

  size_t tmp_idx = idx;
  if((*(this->rule))(argv, argc, tmp_idx, parser))
    {
      idx = tmp_idx;
    }

  return true;
}

std::string
OptionalRule::dump()
{
  stringstream out;

  out << this->rule->dump() << "?";

  return out.str();
}

Parser::Parser(RuleInterface &rule)
  : rule(&rule)
{
}

Parser::~Parser()
{
  delete this->rule;
}


bool
Parser::parse(const char *argv[], int argc)
{
  size_t idx = 1;

  if(not (*(this->rule))(argv, argc, idx, *this))
    return false;

  if(idx != (size_t)argc)
    {
      return false;
    }

  return true;
}

std::string
Parser::format_help(std::string prg_name)
{
  stringstream out;
  out << "Usage: " << prg_name << " " << this->rule->dump() << endl;

  return out.str();
}

void
Parser::set_flag(std::string name)
{
  this->present_flags.insert(name);
}

bool
Parser::has_flag(std::string name)
{
  return 0 != this->present_flags.count(name);
}


void
Parser::set_option(std::string name, std::string value)
{
  if (0 == this->present_options.count(name)) {
    this->present_options[name] = std::list<std::string>();
  }
  this->present_options[name].push_back(value);
}

bool
Parser::has_option(std::string name)
{
  return 0 != this->present_options.count(name);
}

const std::list<std::string> &
Parser::get_option(std::string name)
{
  return this->present_options[name];
}

const std::list<string> &
Parser::get_values()
{
  return this->given_values;
}

void
Parser::add_value(std::string value)
{
  this->given_values.push_back(value);
}



ZeroOrMoreRule::ZeroOrMoreRule(RuleInterface *rule)
: rule(rule)
{
}

ZeroOrMoreRule::~ZeroOrMoreRule()
{
  delete this->rule;
}

bool
ZeroOrMoreRule::operator()(const char *argv[], int argc, size_t &idx, Parser &parser)
{
  if(idx == (size_t)argc)
    return true;

  size_t tmp_idx = idx;
  while((*(this->rule))(argv, argc, tmp_idx, parser))
    {
      idx = tmp_idx;
    }

  return true;
}


std::string
ZeroOrMoreRule::dump()
{
  stringstream out;

  out << this->rule->dump() << "*";

  return out.str();
}
