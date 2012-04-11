#include "benchmark.hh"
#include <iostream>
#include <sstream>
#include <limits>
#include <cmath>

using namespace Fluc::Benchmark;



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




/* ********************************************************************************************* *
 * Implementation of TestSuite
 * ********************************************************************************************* */
TestSuite::TestSuite(const std::string &desc)
  : description(desc)
{
  // pass...
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
  : stream(stream)
{
  // Pass...
}


void
TestRunner::addSuite(TestSuite *suite)
{
  this->suites.push_back(suite);
}


void
TestRunner::operator ()()
{
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
        // Run test
        double mean_time = (**test)();
        this->stream << " ok "
                     << "(" << mean_time << " s/it) "
                     << "(" << 1./mean_time << " it/s)" << std::endl;
      }
      catch (std::exception &err)
      {
        this->stream << " exception" << std::endl;
        this->stream << "   what(): " << err.what() << std::endl;
      }
    }

    this->stream << std::endl;
  }
}

