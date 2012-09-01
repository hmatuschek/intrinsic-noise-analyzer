#include "lexer.hh"
#include <parser/lexer.hh>


using namespace iNA;
using namespace iNA::Parser::Sbmlsh;


Parser::Sbmlsh::Lexer::Lexer(std::istream &input)
  : iNA::Parser::Lexer(input)
{
  // Token rules for white spaces
  this->addRule(new Parser::WhiteSpaceTokenRule(T_WHITESPACE));
  this->addTokenName(T_WHITESPACE, "WHITESPACE");
  this->addRule(new Parser::EOLTokenRule(T_END_OF_LINE));
  this->addTokenName(T_END_OF_LINE, "END_OF_LINE");

  // Tokenrules for keywords
  addRule(new Parser::KeyWordTokenRule(T_MODEL_KW, "@model"));
  addTokenName(T_MODEL_KW, "@model");
  addRule(new Parser::KeyWordTokenRule(T_UNITS_KW, "@units"));
  addTokenName(T_UNITS_KW, "@units");
  addRule(new Parser::KeyWordTokenRule(T_COMPARTMENTS_KW, "@compartments"));
  addTokenName(T_COMPARTMENTS_KW, "@compartments");
  this->addRule(new Parser::KeyWordTokenRule(T_SPECIES_KW, "@species"));
  this->addTokenName(T_SPECIES_KW, "@species");
  this->addRule(new Parser::KeyWordTokenRule(T_PARAMETERS_KW, "@parameters"));
  this->addTokenName(T_PARAMETERS_KW, "@parameters");
  this->addRule(new Parser::KeyWordTokenRule(T_RULES_KW, "@rules"));
  this->addTokenName(T_RULES_KW, "@rules");
  this->addRule(new Parser::KeyWordTokenRule(T_ASSIGN_KW, "@assign"));
  this->addTokenName(T_ASSIGN_KW, "@assgin");
  this->addRule(new Parser::KeyWordTokenRule(T_RATE_KW, "@rate"));
  this->addTokenName(T_RATE_KW, "@rate");
  this->addRule(new Parser::KeyWordTokenRule(T_REACTIONS_KW, "@reactions"));
  this->addTokenName(T_REACTIONS_KW, "@reactions");
  this->addRule(new Parser::KeyWordTokenRule(T_R_KW, "@r"));
  this->addTokenName(T_R_KW, "@r");
  this->addRule(new Parser::KeyWordTokenRule(T_RR_KW, "@rr"));
  this->addTokenName(T_RR_KW, "@rr");
  this->addRule(new Parser::KeyWordTokenRule(T_EVENTS_KW, "@events"));
  this->addTokenName(T_EVENTS_KW, "@events");

  this->addRule(new VersionNumberTokenRule());
  this->addTokenName(T_VERSION_NUMBER, "VERSION_NUMBER");

  this->addRule(new Parser::IntegerTokenRule(T_INTEGER));
  this->addTokenName(T_INTEGER, "INTEGER");
  this->addRule(new Parser::FloatTokenRule(T_FLOAT));
  this->addTokenName(T_FLOAT, "FLOAT");

  this->addRule(new Parser::IdentifierTokenRule(T_IDENTIFIER));
  this->addTokenName(T_IDENTIFIER, "IDENTIFIER");
  this->addRule(new Parser::StringTokenRule(T_QUOTED_STRING));
  this->addTokenName(T_QUOTED_STRING, "STRING");

  this->addRule(new Parser::KeyWordTokenRule(T_COLON, ":"));
  this->addTokenName(T_COLON, "COLON");
  this->addRule(new Parser::KeyWordTokenRule(T_SEMICOLON, ";"));
  this->addTokenName(T_SEMICOLON, "SEMICOLON");
  this->addRule(new Parser::KeyWordTokenRule(T_COMMA, ","));
  this->addTokenName(T_COMMA, "COMMA");

  this->addRule(new Parser::KeyWordTokenRule(T_ASSIGN, "="));
  this->addTokenName(T_ASSIGN, "OP_ASSIGN");
  this->addRule(new Parser::KeyWordTokenRule(T_EQUAL, "=="));
  this->addTokenName(T_EQUAL, "OP_EQUAL");
  this->addRule(new Parser::KeyWordTokenRule(T_NEQUAL, "!="));
  this->addTokenName(T_NEQUAL, "OP_NOT_EQUAL");
  this->addRule(new Parser::KeyWordTokenRule(T_LESSTHAN, "<"));
  this->addTokenName(T_LESSTHAN, "OP_LESS_THAN");
  this->addRule(new Parser::KeyWordTokenRule(T_LESSEQUAL, "<="));
  this->addTokenName(T_LESSEQUAL, "OP_LESS_EQUAL");
  this->addRule(new Parser::KeyWordTokenRule(T_GREATERTHAN, ">"));
  this->addTokenName(T_GREATERTHAN, "OP_GREATER_THAN");
  this->addRule(new Parser::KeyWordTokenRule(T_GREATEREQUAL, ">="));
  this->addTokenName(T_GREATEREQUAL, "OP_GREATER_EQUAL");

  this->addRule(new Parser::KeyWordTokenRule(T_PLUS, "+"));
  this->addTokenName(T_PLUS, "OP_PLUS");
  this->addRule(new Parser::KeyWordTokenRule(T_MINUS, "-"));
  this->addTokenName(T_MINUS, "OP_MINUS");
  this->addRule(new Parser::KeyWordTokenRule(T_TIMES, "*"));
  this->addTokenName(T_TIMES, "OP_TIMES");
  this->addRule(new Parser::KeyWordTokenRule(T_POWER, "**"));
  this->addRule(new Parser::KeyWordTokenRule(T_POWER, "^"));
  this->addTokenName(T_POWER, "OP_POWER");
  this->addRule(new Parser::KeyWordTokenRule(T_DIVIVE, "/"));
  this->addTokenName(T_DIVIVE, "OP_DIVIDE");
  this->addRule(new Parser::KeyWordTokenRule(T_RARROW, "->"));
  this->addTokenName(T_RARROW, "OP_RIGHT_ARROW");
  this->addRule(new Parser::KeyWordTokenRule(T_LPAR, "("));
  this->addTokenName(T_LPAR, "LPAR");
  this->addRule(new Parser::KeyWordTokenRule(T_RPAR, ")"));
  this->addTokenName(T_RPAR, "RPAR");
  this->addRule(new Parser::KeyWordTokenRule(T_LBRAC, "["));
  this->addTokenName(T_LBRAC, "LBRAC");
  this->addRule(new Parser::KeyWordTokenRule(T_RBRAC, "]"));
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
  : TokenRule(T_VERSION_NUMBER)
{
  State *si = createState(false);
  State *s1 = createState(false);
  State *s2 = createState(false);
  State *s3 = createState(true);
  onNumber(si, si);
  onChar('.', si, s1);
  onNumber(s1, s1);
  onChar('.', s1, s2);
  onNumber(s2, s3);
  onNumber(s3,s3);
}
