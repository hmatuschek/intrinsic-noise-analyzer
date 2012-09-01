#ifndef __FLUC_UTILS_UNITTEST_HH__
#define __FLUC_UTILS_UNITTEST_HH__

#include <list>
#include <string>
#include <ginac/ginac.h>


namespace iNA {
namespace Utils {
namespace UnitTest {


/**
 * Base exception to signal the failure of a test-case.
 *
 * @ingroup utils
 */
class TestFailure : public std::exception
{
protected:
  /**
   * Holds the error message.
   */
  std::string message;

public:
  /**
   * Constructor.
   */
  TestFailure(const std::string &message) throw();

  /**
   * Destructor.
   */
  virtual ~TestFailure() throw();

  /**
   * Returns the message as a char pointer.
   */
  const char *what() const throw();
};



/**
 * Base class of all test-cases.
 *
 * @ingroup utils
 */
class TestCase
{
public:
  /**
   * Will be called just before each test.
   */
  virtual void setUp();

  /**
   * Will be called just after each test.
   */
  virtual void tearDown();

  /**
   * Asserts, the given expression @c test is true.
   */
  void assertTrue(bool test, const std::string &file, size_t line);

  /**
   * Asserts, the two values are equal.
   */
  void assertEqual(bool t, bool e, const std::string &file, size_t line);

  /**
   * Asserts, the two values are equal.
   */
  void assertEqual(char t, char e, const std::string &file, size_t line);

  /**
   * Asserts, the two values are equal.
   */
  void assertEqual(short t, short e, const std::string &file, size_t line);

  /**
   * Asserts, the two values are equal.
   */
  void assertEqual(int t, int e, const std::string &file, size_t line);

  /**
   * Asserts, the two values are equal.
   */
  void assertEqual(long t, long e, const std::string &file, size_t line);

  /**
   * Asserts, the two values are equal.
   */
  void assertEqual(unsigned char t, unsigned char e, const std::string &file, size_t line);

  /**
   * Asserts, the two values are equal.
   */
  void assertEqual(unsigned short t, unsigned short e, const std::string &file, size_t line);

  /**
   * Asserts, the two values are equal.
   */
  void assertEqual(unsigned int t, unsigned int e, const std::string &file, size_t line);

  /**
   * Asserts, the two values are equal.
   */
  void assertEqual(unsigned long t, unsigned long e, const std::string &file, size_t line);

  /**
   * Asserts, the two values are equal.
   */
  void assertEqual(float t, float e, const std::string &file, size_t line);

  /**
   * Asserts, the two values are equal.
   */
  void assertEqual(const GiNaC::ex &t,const GiNaC::ex &e, const std::string &file, size_t line);

  /**
   * Asserts, the two values are equal.
   */
  void assertEqual(double t, double e, const std::string &file, size_t line);

  /**
   * Asserts, the two values are equal.
   */
  void assertEqual(const std::string &t, const std::string &e, const std::string &file, size_t line);

  /**
   * Asserts, the two values are almost equal.
   */
  void assertNear(float t, float e, const std::string &file, size_t line);

  /**
   * Asserts, the two values are almost equal.
   */
  void assertNear(double t, double e, const std::string &file, size_t line);
};



/**
 * Base interface for tests, internal used.
 *
 * @ingroup utils
 */
class TestCallerInterface
{
protected:
  /**
   * Description of the test.
   */
  std::string description;

public:
  /**
   * Constructor.
   */
  TestCallerInterface(const std::string &desc)
    : description(desc)
  {
    // Pass...
  }
  /**
   * Virtual destructor.
   */
  virtual ~TestCallerInterface()
  {
    // Pass...
  }

  /**
   * Returns the description of the test.
   */
  virtual const std::string &getDescription()
  {
    return this->description;
  }

  /**
   * Needs to be overridded to execute the test.
   */
  virtual void operator() () = 0;
};



/**
 * Template for test as methods of a @c TestCase.
 *
 * @ingroup utils
 */
template <class T>
class TestCaller : public TestCallerInterface
{
protected:
  /**
   * Pointer to a member function of template-class.
   */
  void (T::*function)(void);


public:
  /**
   * Constructor.
   */
  TestCaller(const std::string &desc, void (T::*func)(void))
    : TestCallerInterface(desc), function(func)
  {
    // Pass...
  }

  /**
   * Calls the referenced test-function.
   */
  virtual void operator() ()
  {
    // Create new test:
    T *instance = new T();

    // Call test
    instance->setUp();
    (instance->*function)();
    instance->tearDown();
  }
};


/**
 * A collection of tests.
 *
 * @ingroup utils
 */
class TestSuite
{
public:
  /**
   * Defines the interator over all tests.
   */
  typedef std::list<TestCallerInterface *>::iterator iterator;

protected:
  /**
   * Description of the test-suite.
   */
  std::string description;

  /**
   * Holds the list of tests.
   */
  std::list<TestCallerInterface *> tests;

public:
  /**
   * Constructor.
   */
  TestSuite(const std::string &desc);

  /**
   * Adds a test.
   */
  void addTest(TestCallerInterface *test);

  /**
   * Returns the description of the test-suite.
   */
  const std::string &getDescription();

  /**
   * Returns an iterator pointing to the first test.
   */
  iterator begin();

  /**
   * Returns an interator pointing right after the last test.
   */
  iterator end();
};



/**
 * Runs tests of one or more @c TestSuite instances.
 *
 * @ingroup utils
 */
class TestRunner
{
protected:
  /**
   * Output stream.
   */
  std::ostream &stream;

  /**
   * A list of test suites.
   */
  std::list<TestSuite *> suites;

public:
  /**
   * Constructor.
   */
  TestRunner(std::ostream &stream);

  /**
   * Adds a test-suite.
   */
  void addSuite(TestSuite *suite);

  /**
   * Executes all test-suites.
   */
  void operator() ();
};


#define UT_ASSERT(t) this->assertTrue(t, __FILE__, __LINE__)
#define UT_ASSERT_EQUAL(t, e) this->assertEqual(t, e, __FILE__, __LINE__)
#define UT_ASSERT_NEAR(t, e) this->assertNear(t, e, __FILE__, __LINE__)
#define UT_ASSERT_THROW(t, e) \
  try { t; throw UnitTest::TestFailure("No exception thrown!"); } catch (e &err) {}

}
}
}

#endif // UNITTEST_HH
