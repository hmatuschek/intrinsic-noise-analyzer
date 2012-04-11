#ifndef INTPRTVSDIRECT_HH
#define INTPRTVSDIRECT_HH

#include "benchmark.hh"
#include "fluctuator.hh"

#include <map>
#include <vector>



namespace Fluc {


class IntprtVsDirect : Benchmark::TestCase
{
protected:
  GiNaC::ex expression;
  GiNaC::exmap value_map;

  Eigen::VectorXd value_vector;
  std::vector<Intprt::Instruction> code;
  std::vector<Intprt::Value> stack;


public:
  virtual void setUp();
  virtual void tearDown();

  void testDirect();
  void testIntprt();


public:
  static Benchmark::TestSuite *suite();
};


}

#endif // INTPRTVSDIRECT_HH
