#include "sbmlsh.hh"
#include <exception.hh>
#include "parser/sbmlsh/parser.hh"
#include "parser/sbmlsh/writer.hh"

#include <fstream>


using namespace iNA;
using namespace iNA::Parser::Sbmlsh;


void
iNA::Parser::Sbmlsh::importModel(Ast::Model &model, std::istream &input)
{
  Parser parser(input);
  parser.parse(model);
}

void
iNA::Parser::Sbmlsh::importModel(Ast::Model &model, const std::string &filename)
{
  std::fstream stream(filename.c_str(), std::ios_base::in);

  if (! stream.is_open()) {
    SBMLParserError err;
    err << "Can not open file \"" << filename << "\".";
    throw err;
  }

  importModel(model, stream);
}

Ast::Model *
iNA::Parser::Sbmlsh::importModel(std::istream &input) {
  Ast::Model *model = new Ast::Model();
  importModel(*model, input);
  return model;
}

Ast::Model *
iNA::Parser::Sbmlsh::importModel(const std::string &filename) {
  Ast::Model *model = new Ast::Model();
  importModel(*model, filename);
  return model;
}



void
iNA::Parser::Sbmlsh::exportModel(Ast::Model &model, std::ostream &stream) {
  Writer::processModel(model, stream);
}


void
iNA::Parser::Sbmlsh::exportModel(Ast::Model &model, const std::string &filename) {
  std::fstream stream(filename.c_str(), std::ios_base::out);

  if (! stream.is_open()) {
    ExportError err;
    err << "Can not open file \"" << filename << "\".";
    throw err;
  }

  Writer::processModel(model, stream);
  stream.close();
}





