#ifndef CONSTANTFOLDERTEST_HH
#define CONSTANTFOLDERTEST_HH

#include "unittest.hh"


namespace Fluc {

/** Simple class to test @c Trafo::ConstantFolder. */
class ConstantFolderTest : public UnitTest::TestCase
{
public:
  void testConstantFolder();

public:
  static UnitTest::TestSuite *suite();
};


}

#endif // CONSTANTFOLDERTEST_HH
