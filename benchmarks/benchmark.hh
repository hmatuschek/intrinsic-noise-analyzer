#ifndef __FLUC_BENCHMARK_HH__
#define __FLUC_BENCHMARK_HH__

#include <list>
#include <string>
#include "cputime.hh"



namespace Fluc {
namespace Benchmark {


class TestCase
{
public:
  virtual void setUp();
  virtual void tearDown();
};



class TestCallerInterface
{
protected:
  std::string description;
  size_t iterations;

public:
  TestCallerInterface(const std::string &desc, size_t iterations)
    : description(desc), iterations(iterations)
  {
    // Pass...
  }

  virtual const std::string &getDescription()
  {
    return this->description;
  }

  virtual double operator() () = 0;
};



template <class T>
class TestCaller : public TestCallerInterface
{
protected:
  void (T::*function)(void);

public:
  TestCaller(const std::string &desc, void (T::*func)(void), size_t iterations=1)
    : TestCallerInterface(desc, iterations), function(func)
  {
    // Pass...
  }

  virtual double operator() ()
  {
    // Create new test:
    T *instance = new T();

    // Create clock:
    CpuTime clock;

    // Setup Test
    instance->setUp();

    // Measure time during execution:
    clock.start();
    for (size_t i=0; i<this->iterations; i++)
    {
      (instance->*function)();
    }
    double time = clock.stop()/this->iterations;

    // Close test
    instance->tearDown();

    return time;
  }
};


class TestSuite
{
public:
  typedef std::list<TestCallerInterface *>::iterator iterator;


protected:
  std::string description;
  std::list<TestCallerInterface *> tests;


public:
  TestSuite(const std::string &desc);

  void addTest(TestCallerInterface *test);

  const std::string &getDescription();

  iterator begin();
  iterator end();
};



class TestRunner
{
protected:
  std::ostream &stream;
  std::list<TestSuite *> suites;

public:
  TestRunner(std::ostream &stream);
  void addSuite(TestSuite *suite);

  void operator() ();
};


#define UT_ASSERT(t) this->assertTrue(t, __FILE__, __LINE__)
#define UT_ASSERT_EQUAL(t, e) this->assertEqual(t, e, __FILE__, __LINE__)
#define UT_ASSERT_NEAR(t, e) this->assertNear(t, e, __FILE__, __LINE__)

}
}


#endif // UNITTEST_HH
