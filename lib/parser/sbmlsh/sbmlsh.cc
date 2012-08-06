#include "sbmlsh.hh"
#include <exception.hh>
#include "parser/sbmlsh/parser.hh"
#include "parser/sbmlsh/exporter.hh"

#include <fstream>


using namespace Fluc;
using namespace Fluc::Parser::Sbmlsh;


void
Fluc::Parser::Sbmlsh::importModel(Ast::Model &model, std::istream &input)
{
  Parser parser(input);
  parser.parse(model);
}

void
Fluc::Parser::Sbmlsh::importModel(Ast::Model &model, const std::string &filename)
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
Fluc::Parser::Sbmlsh::importModel(std::istream &input) {
  Ast::Model *model = new Ast::Model();
  importModel(*model, input);
  return model;
}

Ast::Model *
Fluc::Parser::Sbmlsh::importModel(const std::string &filename) {
  Ast::Model *model = new Ast::Model();
  importModel(*model, filename);
  return model;
}



void
Fluc::Parser::Sbmlsh::exportModel(Ast::Model &model, std::ostream &stream) {
  Writer::processModel(model, stream);
}


void
Fluc::Parser::Sbmlsh::exportModel(Ast::Model &model, const std::string &filename) {
  std::fstream stream(filename.c_str(), std::ios_base::out);

  if (! stream.is_open()) {
    ExportError err;
    err << "Can not open file \"" << filename << "\".";
    throw err;
  }

  Writer::processModel(model, stream);
  stream.close();
}





