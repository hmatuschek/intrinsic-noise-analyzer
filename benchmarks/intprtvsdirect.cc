#include "intprtvsdirect.hh"

using namespace Fluc;


void
IntprtVsDirect::setUp()
{
  GiNaC::symbol a("a"), b("b"), c("c"), d("d");
  this->expression = 4*pow(a,3)*pow(b,2)*pow(c,1)*d;

  // Associate symbol -> index
  std::map<GiNaC::symbol, size_t , GiNaC::ex_is_less> symbol_table;
  symbol_table[a] = 0; symbol_table[b] = 1;
  symbol_table[c] = 2; symbol_table[d] = 3;

  // assign values to symbols:
  this->value_vector.resize(4);
  this->value_vector << 1, 2, 3, 4;

  this->value_map[a] = 1; this->value_map[b] = 2;
  this->value_map[c] = 3; this->value_map[d] = 4;

  // Pre-allocate some space on the stack:
  this->stack.reserve(256);

  std::list<Intprt::Instruction> code;
  Intprt::AssemblerByIndex assembler(code, this->value_vector, symbol_table);
  this->expression.accept(assembler);

  this->code.resize(code.size()); size_t i=0;
  for (std::list<Intprt::Instruction>::iterator iter = code.begin();
       iter != code.end(); iter++, i++)
  {
    this->code[i] = *iter;
  }
}


void
IntprtVsDirect::tearDown()
{

}


void
IntprtVsDirect::testDirect()
{
  double value =
      GiNaC::ex_to<GiNaC::numeric>(GiNaC::evalf(this->expression.subs(this->value_map))).to_double();
  value = value;
}


void
IntprtVsDirect::testIntprt()
{
  // Evaluate:
  for (size_t i=0; i<this->code.size(); i++)
  {
    this->code[i].eval(stack);
  }

  double value = this->stack.back().asFloat(); this->stack.pop_back();
  value = value;
}


Benchmark::TestSuite *
IntprtVsDirect::suite()
{
  Benchmark::TestSuite *s = new Benchmark::TestSuite("Interpreter Test");

  s->addTest(new Benchmark::TestCaller<IntprtVsDirect>("test direct evaluation",
                                                       &IntprtVsDirect::testDirect, 10000));

  s->addTest(new Benchmark::TestCaller<IntprtVsDirect>("test stack machine",
                                                       &IntprtVsDirect::testIntprt, 10000));

  return s;
}
