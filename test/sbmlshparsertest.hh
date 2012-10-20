#ifndef __FLUC_SBMLSHPARSERTEST_HH__
#define __FLUC_SBMLSHPARSERTEST_HH__

#include "unittest.hh"


namespace iNA {

/** Simple test for the SBML-SH parser. */
class SBMLSHParserTest : public UnitTest::TestCase
{
public:
  /** Tests lexical analysis of identifier. */
  void testLexerIdentifier();
  /** Tests lexical analysis of strings. */
  void testLexerString();
  /** Tests lexical analyis of integers. */
  void testLexerInteger();
  /** Tests lexical analyis of floats. */
  void testLexerFloat();
  /** Tests lexical analysis of floats. */
  void testLexerExpFloat();
  /** Tests lexical analysis of some keywords. */
  void testLexerKeywords();
  /** Tests parsing of model-definitions. */
  void testParserModelDefinition();
  /** Tests parsing of unit-definitions. */
  void testParserUnitDefinition();
  /** Tests parsing of compartment-definitions. */
  void testParserCompartmentDefinition();
  /** Tests parsing of species-definitions. */
  void testParserSpeciesDefinition();
  /** Tests parsing of complete models. */
  void testParserModel();
  /** Tests if definition order of variables is insigificant. */
  void testVarDefOrder();


public:
  /** Creates test suite. */
  static UnitTest::TestSuite *suite();
};

}

#endif // SBMLSHPARSERTEST_HH
