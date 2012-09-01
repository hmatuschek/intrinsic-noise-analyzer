#include "parser.hh"
#include "ast/model.hh"
#include "assembler.hh"
#include <fstream>

using namespace iNA;
using namespace iNA::Parser::Sbmlsh;


/* ******************************************************************************************** *
 * Implementation of Parser:
 * ******************************************************************************************** */
iNA::Parser::Sbmlsh::Parser::Parser(std::istream &input)
  : lexer(input), grammar(0)
{
  // Instantiate Grammar
  grammar = ModelProduction::get();
}


void
iNA::Parser::Sbmlsh::Parser::parse(Ast::Model &model)
{
  // Reset lexer:
  lexer.reset();

  // Try to parse using grammar:
  iNA::Parser::ConcreteSyntaxTree root;
  // First parse the grammar
  grammar->parse(lexer, root);
  // now, assemble CST
  grammar->notify(lexer, root);

  // Assemble model from CST
  Assembler assembler(model, lexer);
  assembler.process(root);
}


Ast::Model *
iNA::Parser::Sbmlsh::Parser::parse()
{
  Ast::Model *model = new Ast::Model();
  parse(*model);
  return model;
}



