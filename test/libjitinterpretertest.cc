#include "libjitinterpretertest.hh"

#include "eval/direct/code.hh"
#include "eval/direct/compiler.hh"
#include "eval/direct/interpreter.hh"

#include "eval/libjit/code.hh"
#include "eval/libjit/compiler.hh"
#include "eval/libjit/interpreter.hh"

using namespace Fluc;

namespace Eigen {
typedef Matrix<std::complex<double>, Dynamic, 1>       VectorXc;
typedef Matrix<std::complex<double>, Dynamic, Dynamic> MatrixXc;
}


void
LibJitInterpreterTest::testComplexSum()
{
  GiNaC::symbol a("a"), b("b");
  GiNaC::ex expr = a + b;

  // Associate symbol -> index
  std::map<GiNaC::symbol, size_t , GiNaC::ex_is_less> index_table;
  index_table[a] = 0; index_table[b] = 1;

  // assign values to symbols:
  Eigen::VectorXc values(2); values << std::complex<double>(1,2), std::complex<double>(3,4);

  // Allocate result vectors:
  Eigen::VectorXc true_result(1);
  Eigen::VectorXc jit_result(1);

  // Compile and eval using direct evaluation:
  {
    Evaluate::direct::Code code;
    Evaluate::direct::Compiler<Eigen::VectorXc> compiler(index_table);
    Evaluate::direct::Interpreter<Eigen::VectorXc> interpreter(&code);
    compiler.setCode(&code); compiler.compileExpressionAndStore(expr, 0); compiler.finalize();
    interpreter.run(values, true_result);
  }

  // Compile and eval using libjit:
  {
    Evaluate::libjit::Code code;
    Evaluate::libjit::Compiler<Eigen::VectorXc> compiler(index_table);
    Evaluate::libjit::Interpreter<Eigen::VectorXc> interpreter(&code);
    compiler.setCode(&code); compiler.compileExpressionAndStore(expr, 0); compiler.finalize();
    interpreter.run(values, jit_result);
  }

  // Compare results:
  UT_ASSERT_EQUAL(jit_result(0), true_result(0));
}



void
LibJitInterpreterTest::testComplexProd()
{
  GiNaC::symbol a("a"), b("b");
  GiNaC::ex expr = a * b;

  // Associate symbol -> index
  std::map<GiNaC::symbol, size_t , GiNaC::ex_is_less> index_table;
  index_table[a] = 0; index_table[b] = 1;

  // assign values to symbols:
  Eigen::VectorXc values(2); values << std::complex<double>(1,2), std::complex<double>(3,4);

  // Allocate result vectors:
  Eigen::VectorXc true_result(1);
  Eigen::VectorXc jit_result(1);

  // Compile and eval using direct evaluation:
  {
    Evaluate::direct::Code code;
    Evaluate::direct::Compiler<Eigen::VectorXc> compiler(index_table);
    Evaluate::direct::Interpreter<Eigen::VectorXc> interpreter(&code);
    compiler.setCode(&code); compiler.compileExpressionAndStore(expr, 0); compiler.finalize();
    interpreter.run(values, true_result);
  }

  // Compile and eval using libjit:
  {
    Evaluate::libjit::Code code;
    Evaluate::libjit::Compiler<Eigen::VectorXc> compiler(index_table);
    Evaluate::libjit::Interpreter<Eigen::VectorXc> interpreter(&code);
    compiler.setCode(&code); compiler.compileExpressionAndStore(expr, 0); compiler.finalize();
    interpreter.run(values, jit_result);
  }

  // Compare results:
  UT_ASSERT_EQUAL(jit_result(0), true_result(0));
}


void
LibJitInterpreterTest::testComplexPow()
{
  GiNaC::symbol a("a"), b("b");
  GiNaC::ex expr = GiNaC::pow(a,2);

  // Associate symbol -> index
  std::map<GiNaC::symbol, size_t , GiNaC::ex_is_less> index_table;
  index_table[a] = 0; index_table[b] = 1;

  // assign values to symbols:
  Eigen::VectorXc values(2); values << std::complex<double>(1,2), std::complex<double>(3,4);

  // Allocate result vectors:
  Eigen::VectorXc true_result(1);
  Eigen::VectorXc jit_result(1);

  // Compile and eval using direct evaluation:
  {
    Evaluate::direct::Code code;
    Evaluate::direct::Compiler<Eigen::VectorXc> compiler(index_table);
    Evaluate::direct::Interpreter<Eigen::VectorXc> interpreter(&code);
    compiler.setCode(&code); compiler.compileExpressionAndStore(expr, 0); compiler.finalize();
    interpreter.run(values, true_result);
  }

  // Compile and eval using libjit:
  {
    Evaluate::libjit::Code code;
    Evaluate::libjit::Compiler<Eigen::VectorXc> compiler(index_table);
    Evaluate::libjit::Interpreter<Eigen::VectorXc> interpreter(&code);
    compiler.setCode(&code); compiler.compileExpressionAndStore(expr, 0); compiler.finalize();
    interpreter.run(values, jit_result);
  }

  // Compare results:
  UT_ASSERT_EQUAL(jit_result(0), true_result(0));
}



UnitTest::TestSuite *
LibJitInterpreterTest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("libjit Interpreter Test");

  s->addTest(new UnitTest::TestCaller<LibJitInterpreterTest>(
               "test complex sum", &LibJitInterpreterTest::testComplexSum));

  s->addTest(new UnitTest::TestCaller<LibJitInterpreterTest>(
               "test complex prod", &LibJitInterpreterTest::testComplexProd));

  s->addTest(new UnitTest::TestCaller<LibJitInterpreterTest>(
               "test complex pow", &LibJitInterpreterTest::testComplexPow));

  return s;
}
