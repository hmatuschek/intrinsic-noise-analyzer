#include "sbmlshparsertest.hh"

#include <sstream>
#include "parser/sbmlsh/lexer.hh"
#include "parser/sbmlsh/parser.hh"


using namespace iNA;
using namespace iNA::Parser;


void
SBMLSHParserTest::testLexerIdentifier()
{
  std::stringstream text;
  text << "abc cde" << std::endl;

  Sbmlsh::Lexer lexer(text);
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_IDENTIFIER));
  UT_ASSERT_EQUAL(lexer.current().getValue(), "abc"); lexer.next();

  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_IDENTIFIER));
  UT_ASSERT_EQUAL(lexer.current().getValue(), "cde"); lexer.next();

  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_END_OF_LINE)); lexer.next();

  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Parser::Token::END_OF_INPUT));
}


void
SBMLSHParserTest::testLexerString()
{
  std::stringstream text;
  text << "\"abc\"";

  Sbmlsh::Lexer lexer(text);
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_QUOTED_STRING));
  UT_ASSERT_EQUAL(lexer.current().getValue(), "\"abc\""); lexer.next();

  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(iNA::Parser::Token::END_OF_INPUT));
}


void
SBMLSHParserTest::testLexerInteger()
{
  std::stringstream text;
  text << "1234";

  Sbmlsh::Lexer lexer(text);
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_INTEGER));
  UT_ASSERT_EQUAL(lexer.current().getValue(), "1234"); lexer.next();
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(iNA::Parser::Token::END_OF_INPUT));
}


void
SBMLSHParserTest::testLexerFloat()
{
  std::stringstream text;
  text << "12.34";

  Sbmlsh::Lexer lexer(text);
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_FLOAT));
  UT_ASSERT_EQUAL(lexer.current().getValue(), "12.34"); lexer.next();
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(iNA::Parser::Token::END_OF_INPUT));
}


void
SBMLSHParserTest::testLexerExpFloat()
{
  std::stringstream text;
  text << "12e34" << std::endl
       << "12e-34" << std::endl
       << "1.2e34" << std::endl
       << "1.2e-34";

  Sbmlsh::Lexer lexer(text);
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_FLOAT));
  UT_ASSERT_EQUAL(lexer.current().getValue(), "12e34"); lexer.next(); lexer.next();

  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_FLOAT));
  UT_ASSERT_EQUAL(lexer.current().getValue(), "12e-34"); lexer.next(); lexer.next();

  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_FLOAT));
  UT_ASSERT_EQUAL(lexer.current().getValue(), "1.2e34"); lexer.next(); lexer.next();

  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_FLOAT));
  UT_ASSERT_EQUAL(lexer.current().getValue(), "1.2e-34"); lexer.next();

  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(iNA::Parser::Token::END_OF_INPUT));
}


void
SBMLSHParserTest::testLexerKeywords()
{
  std::stringstream text;
  text << "@model" << std::endl
       << "@units" << std::endl
       << "@compartments" << std::endl
       << "@species" << std::endl
       << "@parameters" << std::endl
       << "@rules" << std::endl
       << "@rate" << std::endl
       << "@reactions" << std::endl
       << "@r" << std::endl
       << "@rr" << std::endl
       << "@events";

  Sbmlsh::Lexer lexer(text);
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_MODEL_KW)); lexer.next(); lexer.next();
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_UNITS_KW)); lexer.next(); lexer.next();
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_COMPARTMENTS_KW)); lexer.next(); lexer.next();
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_SPECIES_KW)); lexer.next(); lexer.next();
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_PARAMETERS_KW)); lexer.next(); lexer.next();
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_RULES_KW)); lexer.next(); lexer.next();
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_RATE_KW)); lexer.next(); lexer.next();
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_REACTIONS_KW)); lexer.next(); lexer.next();
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_R_KW)); lexer.next(); lexer.next();
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_RR_KW)); lexer.next(); lexer.next();
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(Sbmlsh::T_EVENTS_KW)); lexer.next();
  UT_ASSERT_EQUAL(lexer.current().getId(), unsigned(iNA::Parser::Token::END_OF_INPUT));
}


void
SBMLSHParserTest::testParserModelDefinition()
{
  {
    std::stringstream text;
    text << "@model:3.3.1 = ModelId \"Model Name\"";

    Sbmlsh::Lexer lexer(text);
    iNA::Parser::Production *prod = Sbmlsh::ModelDefinitionProduction::get();

    iNA::Parser::ConcreteSyntaxTree element;
    prod->parse(lexer, element);
  }

  {
    std::stringstream text;
    text << "@model:3.3.1 = ModelId \"Model Name\"" << std::endl
         << "  s=\"mole\" t=\"seconds\" v=\"litre\"";

    Sbmlsh::Lexer lexer(text);
    iNA::Parser::Production *prod = Sbmlsh::ModelDefinitionProduction::get();
    iNA::Parser::ConcreteSyntaxTree element;
    prod->parse(lexer, element);
  }
}



void
SBMLSHParserTest::testParserUnitDefinition()
{
  {
    std::stringstream text;
    text << "@units" << std::endl
         << "  " << "mmol = mole:s=-3;" << std::endl
         << "  " << "mmoll = mole:s=-3; litre;" << std::endl
         << "  " << "mm = metre:s=-3;";

    Sbmlsh::Lexer lexer(text);
    iNA::Parser::Production *prod = Sbmlsh::UnitDefinitionsProduction::get();
    iNA::Parser::ConcreteSyntaxTree element;
    prod->parse(lexer, element);
  }

  {
    std::stringstream text;
    text << "@units" << std::endl
         << "  " << "mm = mat:s=-3;";

    Sbmlsh::Lexer lexer(text);
    iNA::Parser::Production *prod = Sbmlsh::UnitDefinitionsProduction::get();
    iNA::Parser::ConcreteSyntaxTree element;
    UT_ASSERT_THROW(prod->parse(lexer, element), Parser::ParserError);
  }
}


void
SBMLSHParserTest::testParserCompartmentDefinition()
{
  {
    std::stringstream text;
    text << "@compartments" << std::endl
         << "  " << "cell = 1 \"Cell\"" << std::endl
         << "  " << "cell \"Cell\"" << std::endl
         << "  " << "cell = 1" << std::endl;

    Sbmlsh::Lexer lexer(text);
    iNA::Parser::Production *prod = Sbmlsh::CompartmentDefinitionsProduction::get();
    iNA::Parser::ConcreteSyntaxTree element;
    prod->parse(lexer, element);
  }
}



void
SBMLSHParserTest::testParserSpeciesDefinition()
{
  {
    std::stringstream text;
    text << "@species" << std::endl
         << "  cell: [s] = 1 s b c \"Species\"" << std::endl
         << "  cell: s   = 1 s b c \"Species\"" << std::endl
         << "  cell: [s]     s b c \"Species\"" << std::endl
         << "  cell: [s] = 1     \"Species\"" << std::endl
         << "  cell: [s] = 1 s b c";

    Sbmlsh::Lexer lexer(text);
    iNA::Parser::Production *prod = Sbmlsh::SpeciesDefinitionsProduction::get();
    iNA::Parser::ConcreteSyntaxTree element;
    prod->parse(lexer, element);
  }
}



void
SBMLSHParserTest::testParserModel()
{
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

    Sbmlsh::Parser parser(text);
    Ast::Model *model = parser.parse();
    delete model;
  }
}


void
SBMLSHParserTest::testVarDefOrder()
{
  std::stringstream text;
  text << "@model:3.3.1 = enzymatic \"Enzymatic Reaction\"" << std::endl
       << std::endl
       << "@compartments" << std::endl
       << "  cytosol = 1e5 \"Cytosol\"" << std::endl
       << std::endl
       << "@species" << std::endl
       << "  cytosol: [ES] = P  \"ES\"" << std::endl
       << "  cytosol: [P]  = 0  \"P\"" << std::endl
       << "  cytosol: [S]  = 10 \"S\"" << std::endl
       << "  cytosol: [E]  = 10 \"E\"" << std::endl
       << std::endl
       << "@reactions" << std::endl
       << "  @r = veq \"veq\"" << std::endl
       << "    E + S -> ES" << std::endl
       << "    cytosol*(k1*E*S - k2*ES): k1=k2, k2=0.2" << std::endl
       << "  @r = vcat \"vcat\"" << std::endl
       << "    ES -> E + P" << std::endl
       << "    cytosol*k1*ES: k1=1" << std::endl;   //

  Sbmlsh::Parser parser(text);
  Ast::Model *model = 0;
  UT_ASSERT_NOTHROW(model = parser.parse());
  if (0 != model) { delete model; }
}


UnitTest::TestSuite *
SBMLSHParserTest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Tests for SBML-sh parser.");

  s->addTest(new UnitTest::TestCaller<SBMLSHParserTest>(
               "Lexer: Integer", &SBMLSHParserTest::testLexerInteger));
  s->addTest(new UnitTest::TestCaller<SBMLSHParserTest>(
               "Lexer: Identifier", &SBMLSHParserTest::testLexerIdentifier));
  s->addTest(new UnitTest::TestCaller<SBMLSHParserTest>(
               "Lexer: String", &SBMLSHParserTest::testLexerString));
  s->addTest(new UnitTest::TestCaller<SBMLSHParserTest>(
               "Lexer: Float", &SBMLSHParserTest::testLexerFloat));
  s->addTest(new UnitTest::TestCaller<SBMLSHParserTest>(
               "Lexer: exp. Float", &SBMLSHParserTest::testLexerExpFloat));
  s->addTest(new UnitTest::TestCaller<SBMLSHParserTest>(
               "Lexer: Keywords", &SBMLSHParserTest::testLexerKeywords));
  s->addTest(new UnitTest::TestCaller<SBMLSHParserTest>(
               "Parser: ModelDefinition", &SBMLSHParserTest::testParserModelDefinition));
  s->addTest(new UnitTest::TestCaller<SBMLSHParserTest>(
               "Parser: UnitDefinition", &SBMLSHParserTest::testParserUnitDefinition));
  s->addTest(new UnitTest::TestCaller<SBMLSHParserTest>(
               "Parser: CompartmentDefinition", &SBMLSHParserTest::testParserCompartmentDefinition));
  s->addTest(new UnitTest::TestCaller<SBMLSHParserTest>(
               "Parser: SpeciesDefinition", &SBMLSHParserTest::testParserSpeciesDefinition));
  s->addTest(new UnitTest::TestCaller<SBMLSHParserTest>(
               "Parser: Model", &SBMLSHParserTest::testParserModel));
  s->addTest(new UnitTest::TestCaller<SBMLSHParserTest>(
               "Parser: Variable definition order.", &SBMLSHParserTest::testVarDefOrder));

  return s;
}

