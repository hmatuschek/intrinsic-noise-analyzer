#ifndef CONSTANTFOLDERTEST_HH
#define CONSTANTFOLDERTEST_HH

#include "ast/ast.hh"
#include "unittest.hh"


namespace iNA {

/** Simple class to test @c Trafo::ConstantFolder. */
class ConstantFolderTest : public UnitTest::TestCase
{
protected:
  Ast::Model *_model;

public:
  virtual ~ConstantFolderTest();

  void setUp();
  void tearDown();

  void testConstantFolder();
  void testFolderFilter();

public:
  static UnitTest::TestSuite *suite();
};


}

#endif // CONSTANTFOLDERTEST_HH
