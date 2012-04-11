#include "parser.hh"

using namespace Fluc;
using namespace Fluc::Sbmlsh;


/* ******************************************************************************************** *
 * Implementation of Parser:
 * ******************************************************************************************** */
Parser::Parser(std::istream &input)
  : lexer(input), grammar(0)
{
  grammar = ModelProduction::get();
}


void
Parser::parse()
{
  // Reset lexer:
  lexer.reset();

  // Try to parse using grammar:
  Utils::ConcreteSyntaxTree root;
  grammar->parse(lexer, root);
  grammar->notify(lexer, root);
}



