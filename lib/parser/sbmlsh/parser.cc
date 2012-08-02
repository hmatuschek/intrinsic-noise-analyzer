#include "parser.hh"
#include "ast/model.hh"
#include "assembler.hh"
#include <fstream>

using namespace Fluc;
using namespace Fluc::Parser::Sbmlsh;


/* ******************************************************************************************** *
 * Implementation of Parser:
 * ******************************************************************************************** */
Fluc::Parser::Sbmlsh::Parser::Parser(std::istream &input)
  : lexer(input), grammar(0)
{
  // Instantiate Grammar
  grammar = ModelProduction::get();
}


void
Fluc::Parser::Sbmlsh::Parser::parse(Ast::Model &model)
{
  // Reset lexer:
  lexer.reset();

  // Try to parse using grammar:
  Fluc::Parser::ConcreteSyntaxTree root;
  // First parse the grammar
  grammar->parse(lexer, root);
  // now, assemble CST
  grammar->notify(lexer, root);

  // Assemble model from CST
  Assembler assembler(model, lexer);
  assembler.process(root);
}


Ast::Model *
Fluc::Parser::Sbmlsh::Parser::parse()
{
  Ast::Model *model = new Ast::Model();
  parse(*model);
  return model;
}



