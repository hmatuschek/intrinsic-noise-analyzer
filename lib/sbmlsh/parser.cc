#include "parser.hh"
#include "ast/model.hh"
#include "assembler.hh"

using namespace Fluc;
using namespace Fluc::Sbmlsh;


/* ******************************************************************************************** *
 * Implementation of Parser:
 * ******************************************************************************************** */
Parser::Parser(std::istream &input)
  : lexer(input), grammar(0)
{
  // Instantiate Grammar
  grammar = ModelProduction::get();
}


void
Parser::parse(Ast::Model &model)
{
  // Reset lexer:
  lexer.reset();

  // Try to parse using grammar:
  Utils::ConcreteSyntaxTree root;
  // First parse the grammar
  grammar->parse(lexer, root);
  // now, assemble CST
  grammar->notify(lexer, root);

  // Assemble model from CST
  Assembler assembler(model, lexer);
  assembler.process(root);
}


Ast::Model *
Parser::parse()
{
  Ast::Model *model = new Ast::Model();
  parse(*model);
  return model;
}



