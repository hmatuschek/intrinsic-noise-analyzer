#include "sbml.hh"
#include "assembler.hh"
#include <sbml/SBMLTypes.h>



using namespace Fluc;



void
Parser::Sbml::importModel(Ast::Model &model, const std::string &filename)
{
  // Read SBML document from file:
  libsbml::SBMLDocument *document = libsbml::readSBMLFromFile(filename.c_str());

  // Check for errors:
  if (0 != document->getNumErrors()) {
    SBMLParserError err;
    err << "Can not parse SBML file " << filename << ": " << document->getError(0)->getMessage();
    throw err;
  }

  // try to convert to SBML level 2 version 4:
  if (! document->setLevelAndVersion(2,4)) {
    SBMLParserError err;
    err << "The model in " << filename << " is not compatible with SBML leven 2 version 4!";
    throw err;
  }

  ParserContext ctx(model);
  __process_model(document->getModel(), ctx);
}


Ast::Model *
Parser::Sbml::importModel(const std::string &filename)
{
  Ast::Model *model = new Ast::Model();
  importModel(*model, filename);
  return model;
}
