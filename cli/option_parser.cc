#include "option_parser.hh"
#include <iostream>
#include <sstream>

using namespace std;
using namespace Fluc;


IOptionRule::~IOptionRule()
{

}


OptionRule::OptionRule(std::string long_name, bool is_flag, char short_name)
  : short_name(short_name), long_name(long_name), is_flag(is_flag)
{
}


bool
OptionRule::operator()(char *argv[], int argc, size_t &idx, OptionParser &parser)
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
    this->value = argv[idx];
    idx++;
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


OptionRuleOneOf::OptionRuleOneOf()
{
}

OptionRuleOneOf::OptionRuleOneOf(std::list<IOptionRule *> &opts)
  : options(opts)
{
}

OptionRuleOneOf::~OptionRuleOneOf()
{
  for(list<IOptionRule *>::iterator it=this->options.begin();
      it != this->options.end(); it++)
    {
      delete *it;
    }
}


bool
OptionRuleOneOf::operator()(char *argv[], int argc, size_t &idx, OptionParser &parser)
{
  for(list<IOptionRule *>::iterator it = this->options.begin();
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
OptionRuleOneOf::dump()
{
  stringstream out;
  list<IOptionRule *>::iterator it=this->options.begin();

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
OptionRuleValue::operator()(char *argv[], int argc, size_t &idx, OptionParser &parser)
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
OptionRuleValue::dump()
{
  return "ANYVALUE";
}


OptionRuleList::OptionRuleList(std::list<IOptionRule *> &opts)
  : options(opts)
{
}

OptionRuleList::~OptionRuleList()
{
  for(list<IOptionRule *>::iterator it=this->options.begin();
      it != this->options.end(); it++)
    {
      delete *it;
    }
}

bool
OptionRuleList::operator()(char *argv[], int argc, size_t &idx, OptionParser &parser)
{
  if(idx == (size_t)argc)
    return false;

  for(list<IOptionRule *>::iterator it=this->options.begin();
      it != this->options.end(); it++)
    {
      if(not (**it)(argv, argc, idx, parser))
  return false;
    }

  return true;
}

std::string
OptionRuleList::dump()
{
  stringstream out;
  list<IOptionRule *>::iterator it=this->options.begin();

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


OptionRuleOptional::OptionRuleOptional(IOptionRule *rule)
  : rule(rule)
{
}

OptionRuleOptional::~OptionRuleOptional()
{
  delete this->rule;
}

bool
OptionRuleOptional::operator()(char *argv[], int argc, size_t &idx, OptionParser &parser)
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
OptionRuleOptional::dump()
{
  stringstream out;

  out << this->rule->dump() << "?";

  return out.str();
}

OptionParser::OptionParser(IOptionRule *rule)
  : rule(rule)
{
}

OptionParser::~OptionParser()
{
  delete this->rule;
}


bool
OptionParser::parse(char *argv[], int argc)
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
OptionParser::format_help(std::string prg_name)
{
  stringstream out;
  out << "Usage: " << prg_name << " " << this->rule->dump() << endl;

  return out.str();
}

void
OptionParser::set_flag(std::string name)
{
  this->present_flags.insert(name);
}

bool
OptionParser::has_flag(std::string name)
{
  return 0 != this->present_flags.count(name);
}


void
OptionParser::set_option(std::string name, std::string value)
{
  this->present_options[name] = value;
}

bool
OptionParser::has_option(std::string name)
{
  return 0 != this->present_options.count(name);
}

std::string
OptionParser::get_option(std::string name)
{
  return this->present_options[name];
}

std::list<string>
OptionParser::get_values()
{
  return this->given_values;
}

void
OptionParser::add_value(std::string value)
{
  this->given_values.push_back(value);
}



OptionRuleZeroOrMore::OptionRuleZeroOrMore(IOptionRule *rule)
: rule(rule)
{
}

OptionRuleZeroOrMore::~OptionRuleZeroOrMore()
{
  delete this->rule;
}

bool
OptionRuleZeroOrMore::operator()(char *argv[], int argc, size_t &idx, OptionParser &parser)
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
OptionRuleZeroOrMore::dump()
{
  stringstream out;

  out << this->rule->dump() << "*";

  return out.str();
}
