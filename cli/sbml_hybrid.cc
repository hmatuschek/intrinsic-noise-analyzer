#include "sbml_hybrid.hh"
#include <parser/sbml/sbml.hh>
#include <fstream>
#include "unsupported.hh"

using namespace iNA;

int main(int argc, char *argv[])
{
  // Check args:
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " SBML_FILENAME" << std::endl;
    return -1;
  }

    std::set<Ast::Species *> exS;

    // Construct hybrid model from SBMLsh model
    Ast::Model sbml_model; Parser::Sbmlsh::importModel(sbml_model, argv[1]);
    Models::HybridModel hm(sbml_model,exS);

}
