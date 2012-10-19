#include "sbml.hh"
#include "assembler.hh"
#include "writer.hh"
#include <sbml/SBMLTypes.h>
#include <trafo/consistencycheck.hh>


using namespace iNA;



void
Parser::Sbml::importModel(Ast::Model &model, const std::string &filename)
{
  // Read SBML document from file:
  LIBSBML_CPP_NAMESPACE_QUALIFIER SBMLDocument *document = LIBSBML_CPP_NAMESPACE_QUALIFIER readSBMLFromFile(filename.c_str());

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

  // Assemble Ast::Model
  ParserContext ctx(model);
  __process_model(document->getModel(), ctx);

  // Check model consistency:
  Trafo::ConsistencyCheck::assertConsistent(&model);
}


Ast::Model *
Parser::Sbml::importModel(const std::string &filename)
{
  Ast::Model *model = new Ast::Model();
  importModel(*model, filename);
  return model;
}


void
Parser::Sbml::exportModel(Ast::Model &model, const std::string &filename)
{
  // Create SBML document leven 2 version 4:
  LIBSBML_CPP_NAMESPACE_QUALIFIER SBMLDocument *document = new LIBSBML_CPP_NAMESPACE_QUALIFIER SBMLDocument(2,4);
  // Assemble SBML model from Ast::Model:
  Writer::processModel(model, document);
  // Write SBML into file:
  LIBSBML_CPP_NAMESPACE_QUALIFIER SBMLWriter writer; writer.writeSBMLToFile(document, filename);
  // free document:
  delete document;
}

