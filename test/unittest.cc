#include "unittest.hh"
#include <iostream>
#include <sstream>
#include <limits>
#include <cmath>
#include "utils/cputime.hh"
#include <execinfo.h>
#include <sys/time.h>

using namespace Fluc::UnitTest;



/* ********************************************************************************************* *
 * Implementation of TestFailure
 * ********************************************************************************************* */
TestFailure::TestFailure(const std::string &message) throw()
  : message(message)
{
  // pass...
}

TestFailure::~TestFailure() throw()
{
  // Pass...
}

const char *
TestFailure::what() const throw ()
{
  return this->message.c_str();
}




/* ********************************************************************************************* *
 * Implementation of TestCase
 * ********************************************************************************************* */
void
TestCase::setUp()
{
  // Pass...
}


void
TestCase::tearDown()
{
  // Pass...
}


void
TestCase::assertTrue(bool test, const std::string &file, size_t line)
{
  if (! test)
  {
    std::stringstream str;
    str << "Assert failed in " << file << " in line " << line;
    throw TestFailure(str.str());
  }
}


void
TestCase::assertEqual(bool t, bool e, const std::string &file, size_t line)
{
  if (e != t)
  {
    std::stringstream str;
    str << "Expected: " << e << " but got: " << t
        << " in file "<< file << " in line " << line;
    throw TestFailure(str.str());
  }
}


void
TestCase::assertEqual(char t, char e, const std::string &file, size_t line)
{
  if (e != t)
  {
    std::stringstream str;
    str << "Expected: " << e << " but got: " << t
        << " in file "<< file << " in line " << line;
    throw TestFailure(str.str());
  }
}


void
TestCase::assertEqual(short t, short e, const std::string &file, size_t line)
{
  if (e != t)
  {
    std::stringstream str;
    str << "Expected: " << e << " but got: " << t
        << " in file "<< file << " in line " << line;
    throw TestFailure(str.str());
  }
}


void
TestCase::assertEqual(int t, int e, const std::string &file, size_t line)
{
  if (e != t)
  {
    std::stringstream str;
    str << "Expected: " << e << " but got: " << t
        << " in file "<< file << " in line " << line;
    throw TestFailure(str.str());
  }
}


void
TestCase::assertEqual(long t, long e, const std::string &file, size_t line)
{
  if (e != t)
  {
    std::stringstream str;
    str << "Expected: " << e << " but got: " << t
        << " in file "<< file << " in line " << line;
    throw TestFailure(str.str());
  }
}


void
TestCase::assertEqual(unsigned char t, unsigned char e, const std::string &file, size_t line)
{
  if (e != t)
  {
    std::stringstream str;
    str << "Expected: " << e << " but got: " << t
        << " in file "<< file << " in line " << line;
    throw TestFailure(str.str());
  }
}


void
TestCase::assertEqual(unsigned short t, unsigned short e, const std::string &file, size_t line)
{
  if (e != t)
  {
    std::stringstream str;
    str << "Expected: " << e << " but got: " << t
        << " in file "<< file << " in line " << line;
    throw TestFailure(str.str());
  }
}


void
TestCase::assertEqual(unsigned int t, unsigned int e, const std::string &file, size_t line)
{
  if (e != t)
  {
    std::stringstream str;
    str << "Expected: " << e << " but got: " << t
        << " in file "<< file << " in line " << line;
    throw TestFailure(str.str());
  }
}


void
TestCase::assertEqual(unsigned long t, unsigned long e, const std::string &file, size_t line)
{
  if (e != t)
  {
    std::stringstream str;
    str << "Expected: " << e << " but got: " << t
        << " in file "<< file << " in line " << line;
    throw TestFailure(str.str());
  }
}


void
TestCase::assertEqual(float t, float e, const std::string &file, size_t line)
{
  if (e != t)
  {
    std::stringstream str;
    str << "Expected: " << e << " but got: " << t
        << " in file "<< file << " in line " << line;
    throw TestFailure(str.str());
  }
}


void
TestCase::assertEqual(double t, double e, const std::string &file, size_t line)
{
  if (e != t)
  {
    std::stringstream str;
    str << "Expected: " << e << " but got: " << t
        << " in file "<< file << " in line " << line;
    throw TestFailure(str.str());
  }
}


void
TestCase::assertEqual(const std::complex<double> &t, const std::complex<double> &e, const std::string &file, size_t line)
{
  if (e != t)
  {
    std::stringstream str;
    str << "Expected: " << e << " but got: " << t
        << " in file "<< file << " in line " << line;
    throw TestFailure(str.str());
  }
}


void
TestCase::assertEqual(const std::string &t, const std::string &e, const std::string &file, size_t line)
{
  if (e != t)
  {
    std::stringstream str;
    str << "Expected: \"" << e << "\" but got: \"" << t << "\""
        << " in file "<< file << " in line " << line;
    throw TestFailure(str.str());
  }
}


void
TestCase::assertEqual(const GiNaC::ex &t, const GiNaC::ex &e, const std::string &file, size_t line)
{
  if (!e.is_equal(t))
  {
      std::stringstream str;
      str << "Ginac expressions not equal"
          << " in file "<< file << " in line " << line;
      throw TestFailure(str.str());
  }
}


void
TestCase::assertNear(float t, float e, const std::string &file, size_t line)
{
  if (std::abs(e-t) > 2*std::max(std::abs(t),std::abs(e))*std::numeric_limits<float>::epsilon())
  {
    std::stringstream str;
    str << "Expected: " << e << " but got: " << t
        << " in file "<< file << " in line " << line;
    throw TestFailure(str.str());
  }
}


void
TestCase::assertNear(double t, double e, const std::string &file, size_t line)
{
  if (std::abs(e-t) > 2*std::max(std::abs(t), std::abs(e))*std::numeric_limits<double>::epsilon())
  {
    std::stringstream str;
    str << "Expected: " << e << " but got: " << t
        << " in file "<< file << " in line " << line;
    throw TestFailure(str.str());
  }
}


void
TestCase::assertNear(double t, double e, double eps, const std::string &file, size_t line)
{
  if (std::abs(e-t) > 2*eps)
  {
    std::stringstream str;
    str << "Expected: " << e << " but got: " << t
        << " in file "<< file << " in line " << line;
    throw TestFailure(str.str());
  }
}


void
TestCase::assertNear(std::complex<double> t, std::complex<double> e, const std::string &file, size_t line)
{
  if (std::abs(e-t) > (4*std::max(std::abs(e),std::abs(t))*std::numeric_limits<double>::epsilon()))
  {
    std::stringstream str;
    str << "Expected: " << e << " but got: " << t
        << " in file "<< file << " in line " << line << std::endl
        << "    diff()=" << std::abs(e-t) << " > "
        << (2*std::max(std::abs(e),std::abs(t))*std::numeric_limits<double>::epsilon());
    throw TestFailure(str.str());
  }
}


void
TestCase::assertNear(std::complex<double> t, std::complex<double> e, double eps, const std::string &file, size_t line)
{
  if (std::abs(e-t) > 2*eps)
  {
    std::stringstream str;
    str << "Expected: " << e << " but got: " << t
        << " in file "<< file << " in line " << line;
    throw TestFailure(str.str());
  }
}




/* ********************************************************************************************* *
 * Implementation of TestSuite
 * ********************************************************************************************* */
TestSuite::TestSuite(const std::string &desc)
  : description(desc)
{
  // pass...
}

TestSuite::~TestSuite()
{
  for(std::list<TestCallerInterface *>::iterator item = this->tests.begin();
      item != this->tests.end(); item++) {
    delete *item;
  }
}


void
TestSuite::addTest(TestCallerInterface *test)
{
  this->tests.push_back(test);
}


const std::string &
TestSuite::getDescription()
{
  return this->description;
}


TestSuite::iterator
TestSuite::begin()
{
  return this->tests.begin();
}


TestSuite::iterator
TestSuite::end()
{
  return this->tests.end();
}




/* ********************************************************************************************* *
 * Implementation of TestSuite
 * ********************************************************************************************* */
TestRunner::TestRunner(std::ostream &stream)
  : stream(stream), suites()
{
  // Pass...
}


TestRunner::~TestRunner()
{
  for (std::list<TestSuite *>::iterator item = this->suites.begin();
       item != this->suites.end(); item++)
  {
    delete *item;
  }
}


void
TestRunner::addSuite(TestSuite *suite)
{
  this->suites.push_back(suite);
}


void
TestRunner::operator ()()
{
  size_t tests_run = 0;
  size_t tests_failed = 0;
  size_t tests_error = 0;

  for (std::list<TestSuite *>::iterator suite = this->suites.begin();
       suite != this->suites.end(); suite++)
  {
    // Dump Suite description
    this->stream << "Suite: " << (*suite)->getDescription() << std::endl;

    // For each test in suite:
    for (TestSuite::iterator test = (*suite)->begin(); test != (*suite)->end(); test++)
    {
      this->stream << " test: " << (*test)->getDescription() << ": ";

      try
      {
        tests_run++;
        Utils::CpuTime cpu_clock; cpu_clock.start();
        Utils::RealTime real_clock; real_clock.start();
        // Run test
        (**test)();
        this->stream << " ok (cpu:" << cpu_clock.stop()
                     << "s, real:" << real_clock.stop() << "s)" << std::endl;
      }
      catch (TestFailure &fail)
      {
        this->stream << " fail" << std::endl;
        this->stream << "   reason: " << fail.what() << std::endl;
        tests_failed++;
      }
      /*catch (std::exception &err)
      {
        this->stream << " exception" << std::endl;
        this->stream << "   what(): " << err.what() << std::endl;
        //this->printTraceBack();
        tests_error++;
      }*/
    }

    this->stream << std::endl;
  }

  this->stream << "Summary: " << tests_failed << " tests failed out of "
               << tests_run - tests_error
               << " (" << 100. * float((tests_run-tests_failed-tests_error))/(tests_run-tests_error)
               << "% passed)." << std::endl << "         Where "
               << tests_error << " tests produced errors." << std::endl;
}



void
TestRunner::printTraceBack()
{
  const size_t max_depth = 100;
  size_t stack_depth;
  void *stack_addrs[max_depth];
  char **stack_strings;

  stack_depth = backtrace(stack_addrs, max_depth);
  stack_strings = backtrace_symbols(stack_addrs, stack_depth);

  this->stream << "Traceback: " << std::endl;
  for (size_t i = 1; i < stack_depth; i++) {
    this->stream << "   " << stack_strings[i] << std::endl;
  }

  free(stack_strings); // malloc()ed by backtrace_symbols
}
