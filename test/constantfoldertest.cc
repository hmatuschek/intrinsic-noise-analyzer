#include "constantfoldertest.hh"

#include "ast/ast.hh"
#include "parser/parser.hh"
#include "trafo/trafo.hh"


using namespace Fluc;



void
ConstantFolderTest::testConstantFolder()
{
  std::stringstream text;

  text << "@model:3.3.1 = enzymatic \"Enzymatic Reaction\"" << std::endl
       << std::endl
       << "@compartments" << std::endl
       << "  cytosol = 1e5 \"Cytosol\"" << std::endl
       << std::endl
       << "@species" << std::endl
       << "  cytosol: [ES] = 0  \"ES\"" << std::endl
       << "  cytosol: [P]  = 0  \"P\"" << std::endl
       << "  cytosol: [S]  = 10 \"S\"" << std::endl
       << "  cytosol: [E]  = 10 \"E\"" << std::endl
       << std::endl
       << "@reactions" << std::endl
       << "  @r = veq \"veq\"" << std::endl
       << "    E + S -> ES" << std::endl
       << "    cytosol*(k1*E*S - k2*ES): k1=1e6, k2=0.2" << std::endl
       << "  @r = vcat \"vcat\"" << std::endl
       << "    ES -> E + P" << std::endl
       << "    cytosol*k1*ES: k1=1" << std::endl;   //

  // parse model
  Ast::Model *model = Parser::Sbmlsh::importModel(text);
  // fold constants
  Trafo::ConstantFolder folder(*model); folder.apply(*model);

  GiNaC::ex E  = model->getSpecies("E")->getSymbol();
  GiNaC::ex S  = model->getSpecies("S")->getSymbol();
  GiNaC::ex ES = model->getSpecies("ES")->getSymbol();

  UT_ASSERT_EQUAL(
        model->getReaction("veq")->getKineticLaw()->getRateLaw(),
        1e5*(1e6*E*S-0.2*ES));

  UT_ASSERT_EQUAL(
        model->getReaction("vcat")->getKineticLaw()->getRateLaw(),
        1e5*ES);
}


UnitTest::TestSuite *
ConstantFolderTest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Tests Trafo::ConstantFolder.");
  s->addTest(new UnitTest::TestCaller<ConstantFolderTest>(
               "const paramter folding", &ConstantFolderTest::testConstantFolder));
  return s;
}
