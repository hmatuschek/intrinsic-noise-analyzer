#include "lexer.hh"

using namespace Fluc;
using namespace Fluc::Parser::Sbmlsh;


Lexer::Lexer(std::istream &input)
  : Utils::Lexer(input)
{
  // Token rules for white spaces
  this->addRule(new Utils::WhiteSpaceTokenRule(T_WHITESPACE));
  this->addTokenName(T_WHITESPACE, "WHITESPACE");
  this->addRule(new Utils::EOLTokenRule(T_END_OF_LINE));
  this->addTokenName(T_END_OF_LINE, "END_OF_LINE");

  // Tokenrules for keywords
  this->addRule(new Utils::KeyWordTokenRule(T_MODEL_KW, "@model"));
  this->addTokenName(T_MODEL_KW, "@model");
  this->addRule(new Utils::KeyWordTokenRule(T_UNITS_KW, "@units"));
  this->addTokenName(T_UNITS_KW, "@units");
  this->addRule(new Utils::KeyWordTokenRule(T_COMPARTMENTS_KW, "@compartments"));
  this->addTokenName(T_COMPARTMENTS_KW, "@compartments");
  this->addRule(new Utils::KeyWordTokenRule(T_SPECIES_KW, "@species"));
  this->addTokenName(T_SPECIES_KW, "@species");
  this->addRule(new Utils::KeyWordTokenRule(T_PARAMETERS_KW, "@parameters"));
  this->addTokenName(T_PARAMETERS_KW, "@parameters");
  this->addRule(new Utils::KeyWordTokenRule(T_RULES_KW, "@rules"));
  this->addTokenName(T_RULES_KW, "@rules");
  this->addRule(new Utils::KeyWordTokenRule(T_ASSIGN_KW, "@assign"));
  this->addTokenName(T_ASSIGN_KW, "@assgin");
  this->addRule(new Utils::KeyWordTokenRule(T_RATE_KW, "@rate"));
  this->addTokenName(T_RATE_KW, "@rate");
  this->addRule(new Utils::KeyWordTokenRule(T_REACTIONS_KW, "@reactions"));
  this->addTokenName(T_REACTIONS_KW, "@reactions");
  this->addRule(new Utils::KeyWordTokenRule(T_R_KW, "@r"));
  this->addTokenName(T_R_KW, "@r");
  this->addRule(new Utils::KeyWordTokenRule(T_RR_KW, "@rr"));
  this->addTokenName(T_RR_KW, "@rr");
  this->addRule(new Utils::KeyWordTokenRule(T_EVENTS_KW, "@events"));
  this->addTokenName(T_EVENTS_KW, "@events");

  this->addRule(new VersionNumberTokenRule());
  this->addTokenName(T_VERSION_NUMBER, "VERSION_NUMBER");

  this->addRule(new Utils::IntegerTokenRule(T_INTEGER));
  this->addTokenName(T_INTEGER, "INTEGER");
  this->addRule(new Utils::FloatTokenRule(T_FLOAT));
  this->addTokenName(T_FLOAT, "FLOAT");

  this->addRule(new Utils::IdentifierTokenRule(T_IDENTIFIER));
  this->addTokenName(T_IDENTIFIER, "IDENTIFIER");
  this->addRule(new Utils::StringTokenRule(T_QUOTED_STRING));
  this->addTokenName(T_QUOTED_STRING, "STRING");

  this->addRule(new Utils::KeyWordTokenRule(T_COLON, ":"));
  this->addTokenName(T_COLON, "COLON");
  this->addRule(new Utils::KeyWordTokenRule(T_SEMICOLON, ";"));
  this->addTokenName(T_SEMICOLON, "SEMICOLON");
  this->addRule(new Utils::KeyWordTokenRule(T_COMMA, ","));
  this->addTokenName(T_COMMA, "COMMA");

  this->addRule(new Utils::KeyWordTokenRule(T_ASSIGN, "="));
  this->addTokenName(T_ASSIGN, "OP_ASSIGN");
  this->addRule(new Utils::KeyWordTokenRule(T_EQUAL, "=="));
  this->addTokenName(T_EQUAL, "OP_EQUAL");
  this->addRule(new Utils::KeyWordTokenRule(T_NEQUAL, "!="));
  this->addTokenName(T_NEQUAL, "OP_NOT_EQUAL");
  this->addRule(new Utils::KeyWordTokenRule(T_LESSTHAN, "<"));
  this->addTokenName(T_LESSTHAN, "OP_LESS_THAN");
  this->addRule(new Utils::KeyWordTokenRule(T_LESSEQUAL, "<="));
  this->addTokenName(T_LESSEQUAL, "OP_LESS_EQUAL");
  this->addRule(new Utils::KeyWordTokenRule(T_GREATERTHAN, ">"));
  this->addTokenName(T_GREATERTHAN, "OP_GREATER_THAN");
  this->addRule(new Utils::KeyWordTokenRule(T_GREATEREQUAL, ">="));
  this->addTokenName(T_GREATEREQUAL, "OP_GREATER_EQUAL");

  this->addRule(new Utils::KeyWordTokenRule(T_PLUS, "+"));
  this->addTokenName(T_PLUS, "OP_PLUS");
  this->addRule(new Utils::KeyWordTokenRule(T_MINUS, "-"));
  this->addTokenName(T_MINUS, "OP_MINUS");
  this->addRule(new Utils::KeyWordTokenRule(T_TIMES, "*"));
  this->addTokenName(T_TIMES, "OP_TIMES");
  this->addRule(new Utils::KeyWordTokenRule(T_POWER, "**"));
  this->addTokenName(T_POWER, "OP_POWER");
  this->addRule(new Utils::KeyWordTokenRule(T_POWER, "^"));
  this->addTokenName(T_POWER, "OP_POWER");
  this->addRule(new Utils::KeyWordTokenRule(T_DIVIVE, "/"));
  this->addTokenName(T_DIVIVE, "OP_DIVIDE");
  this->addRule(new Utils::KeyWordTokenRule(T_RARROW, "->"));
  this->addTokenName(T_RARROW, "OP_RIGHT_ARROW");
  this->addRule(new Utils::KeyWordTokenRule(T_LPAR, "("));
  this->addTokenName(T_LPAR, "LPAR");
  this->addRule(new Utils::KeyWordTokenRule(T_RPAR, ")"));
  this->addTokenName(T_RPAR, "RPAR");
  this->addRule(new Utils::KeyWordTokenRule(T_LBRAC, "["));
  this->addTokenName(T_LBRAC, "LBRAC");
  this->addRule(new Utils::KeyWordTokenRule(T_RBRAC, "]"));
  this->addTokenName(T_RBRAC, "RBRAC");

  // Ignore white-spaces:
  this->ignored_token.insert(T_WHITESPACE);

  // New-Line-Token:
  this->new_line_token.insert(T_END_OF_LINE);
}


/* ********************************************************************************************* *
 * Implementation of VersionNumberTokenRule
 * ********************************************************************************************* */
VersionNumberTokenRule::VersionNumberTokenRule()
  : TokenRule(T_VERSION_NUMBER, false)
{
  Utils::State *si = this->getInitialState();
  Utils::State *s1 = this->newState(false);
  Utils::State *s2 = this->newState(false);
  Utils::State *s3 = this->newState(true);
  si->onNumber(si);
  si->onChar('.', s1);
  s1->onNumber(s1);
  s1->onChar('.',s2);
  s2->onNumber(s3);
  s3->onNumber(s3);
}
