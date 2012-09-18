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
  : _lexer(input), _grammar(0)
{
  // Instantiate Grammar
  _grammar = ModelProduction::get();
}


void
iNA::Parser::Sbmlsh::Parser::parse(Ast::Model &model)
{
  // Reset lexer:
  _lexer.reset();

  // Try to parse using grammar:
  iNA::Parser::ConcreteSyntaxTree root;
  // First parse the grammar
  _grammar->parse(_lexer, root);

  // Assemble model from CST
  Assembler assembler(model, _lexer);
  assembler.process(root);
}


Ast::Model *
iNA::Parser::Sbmlsh::Parser::parse()
{
  Ast::Model *model = new Ast::Model();
  parse(*model);
  return model;
}



