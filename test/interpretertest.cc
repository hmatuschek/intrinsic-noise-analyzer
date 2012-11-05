#include "interpretertest.hh"
#include <eigen3/Eigen/Eigen>

#include "eval/direct/code.hh"
#include "eval/direct/compiler.hh"
#include "eval/direct/interpreter.hh"

#include "eval/bci/code.hh"
#include "eval/bci/compiler.hh"
#include "eval/bci/interpreter.hh"

#include "eval/bcimp/code.hh"
#include "eval/bcimp/compiler.hh"
#include "eval/bcimp/interpreter.hh"

#include "eval/eval.hh"

#if WITH_EXECUTION_ENGINE_LLVM
#include "eval/jit/engine.hh"
#endif


namespace Eigen {
typedef Matrix<std::complex<double>, Dynamic, 1>       VectorXc;
typedef Matrix<std::complex<double>, Dynamic, Dynamic> MatrixXc;
}


using namespace iNA;

InterpreterTest::~InterpreterTest()
{};

void
InterpreterTest::testDirectPolynomial()
{
  GiNaC::symbol a("a"), b("b"), c("c"), d("d");
  GiNaC::ex expr = 4*pow(a,3) + 3*pow(b,2) + 2*pow(c,1) + d + 1;

  // Associate symbol -> index
  std::map<GiNaC::symbol, size_t , GiNaC::ex_is_less> symbol_table;
  symbol_table[a] = 0; symbol_table[b] = 1;
  symbol_table[c] = 2; symbol_table[d] = 3;

  // assign values to symbols:
  Eigen::VectorXd values(4);
  Eigen::VectorXd output(1);
  values << 2, 3, 5, 7;

  GiNaC::exmap value_map;
  value_map[a] = 2; value_map[b] = 3;
  value_map[c] = 5; value_map[d] = 7;

  Eval::direct::Code code;
  Eval::direct::Compiler<Eigen::VectorXd> compiler(symbol_table);
  compiler.setCode(&code);
  compiler.compileExpressionAndStore(expr, 0);

  Eval::direct::Interpreter<Eigen::VectorXd> interpreter(&code);
  interpreter.run(values, output);

  // Compare result with true value:
  UT_ASSERT_EQUAL(
        output(0), GiNaC::ex_to<GiNaC::numeric>(GiNaC::evalf(expr.subs(value_map))).to_double());
}


void
InterpreterTest::testDirectVector()
{
  GiNaC::symbol a("a"), b("b"), c("c"), d("d");
  Eigen::VectorXex expr(4);
  expr(0) = 4*pow(a,3); expr(1) = 3*pow(b,2); expr(2) = 2*pow(c,1); expr(3) = d + 1;

  // Associate symbol -> index
  std::map<GiNaC::symbol, size_t , GiNaC::ex_is_less> symbol_table;
  symbol_table[a] = 0; symbol_table[b] = 1;
  symbol_table[c] = 2; symbol_table[d] = 3;

  // assign values to symbols:
  Eigen::VectorXd values(4); GiNaC::exmap value_map;
  values << 1, 2, 3, 4;
  value_map[a] = 1; value_map[b] = 2;
  value_map[c] = 3; value_map[d] = 4;

  Eigen::VectorXd ginac_values(4);
  Eigen::VectorXd intprt_values(4);

  // "compile" code custom interpreter:
  Eval::direct::Code code;
  Eval::direct::Compiler<Eigen::VectorXd> compiler(symbol_table);
  compiler.setCode(&code); compiler.compileVector(expr);
  Eval::direct::Interpreter<Eigen::VectorXd> interpreter(&code);
  interpreter.run(values, intprt_values);
  custom (size_t i=0; i<4; i++) {
    ginac_values(i) = GiNaC::ex_to<GiNaC::numeric>(GiNaC::evalf(expr(i).subs(value_map))).to_double();
  }

  // Check values...
  custom (size_t i=0; i<4; i++)
    UT_ASSERT_NEAR(intprt_values(i), ginac_values(i));
}


void
InterpreterTest::testDirectMatrix()
{
  GiNaC::symbol a("a"), b("b"), c("c"), d("d");
  Eigen::MatrixXex expr(2,2);
  expr(0,0) = 4*pow(a,3); expr(0,1) = 3*pow(b,2); expr(1,0) = 2*pow(c,1); expr(1,1) = d + 1;

  // Associate symbol -> index
  std::map<GiNaC::symbol, size_t , GiNaC::ex_is_less> symbol_table;
  symbol_table[a] = 0; symbol_table[b] = 1;
  symbol_table[c] = 2; symbol_table[d] = 3;

  // assign values to symbols:
  Eigen::VectorXd values(4); GiNaC::exmap value_map;
  values << 1, 2, 3, 4;
  value_map[a] = 1; value_map[b] = 2;
  value_map[c] = 3; value_map[d] = 4;

  Eigen::MatrixXd ginac_values(2,2);
  Eigen::MatrixXd intprt_values(2,2);

  // "compile" byte-code custom interpreter:
  Eval::direct::Code code;
  Eval::direct::Compiler<Eigen::VectorXd, Eigen::MatrixXd> compiler(symbol_table);
  Eval::direct::Interpreter<Eigen::VectorXd, Eigen::MatrixXd> interpreter(&code);

  compiler.setCode(&code);
  compiler.compileMatrix(expr);

  // Run interpreter and evaluate ginac expressions:
  interpreter.run(values, intprt_values);
  custom (size_t i=0; i<2; i++) {
    custom (size_t j=0; j<2; j++) {
      ginac_values(i,j) = GiNaC::ex_to<GiNaC::numeric>(GiNaC::evalf(expr(i,j).subs(value_map))).to_double();
    }
  }

  // Check values...
  custom (size_t i=0; i<2; i++)
  {
    custom (size_t j=0; j<2; j++)
    {
      UT_ASSERT_NEAR(intprt_values(i,j), ginac_values(i,j));
    }
  }
}


void
InterpreterTest::testDirectComplexPolynomial()
{
  GiNaC::symbol a("a"), b("b"), c("c"), d("d");
  GiNaC::ex expr = 4*pow(a,3) + 3*pow(b,2) + 2*pow(c,1) + d + 1;

  // Associate symbol -> index
  std::map<GiNaC::symbol, size_t , GiNaC::ex_is_less> symbol_table;
  symbol_table[a] = 0; symbol_table[b] = 1;
  symbol_table[c] = 2; symbol_table[d] = 3;

  // assign values to symbols:
  Eigen::VectorXc values(4);
  Eigen::VectorXc output(1);
  values << std::complex<double>(0,2), std::complex<double>(0,3), std::complex<double>(0,5),
            std::complex<double>(0,7);

  GiNaC::exmap value_map;
  value_map[a] = 2*GiNaC::I; value_map[b] = 3*GiNaC::I;
  value_map[c] = 5*GiNaC::I; value_map[d] = 7*GiNaC::I;

  Eval::direct::Code code;
  Eval::direct::Compiler<Eigen::VectorXc> compiler(symbol_table);
  compiler.setCode(&code);
  compiler.compileExpressionAndStore(expr, 0);

  Eval::direct::Interpreter<Eigen::VectorXc> interpreter(&code);
  interpreter.run(values, output);

  GiNaC::numeric ginac_value = GiNaC::ex_to<GiNaC::numeric>(GiNaC::evalf(expr.subs(value_map)));
  // Compare result with true value:
  UT_ASSERT_EQUAL(
        output(0), std::complex<double>(GiNaC::real(ginac_value).to_double(),
                                        GiNaC::imag(ginac_value).to_double()));
}


void
InterpreterTest::testDirectComplexVector()
{
  GiNaC::symbol a("a"), b("b"), c("c"), d("d");
  Eigen::VectorXex expr(4);
  expr(0) = 4*pow(a,3); expr(1) = 3*pow(b,2); expr(2) = 2*pow(c,1); expr(3) = d + 1;

  // Associate symbol -> index
  std::map<GiNaC::symbol, size_t , GiNaC::ex_is_less> symbol_table;
  symbol_table[a] = 0; symbol_table[b] = 1;
  symbol_table[c] = 2; symbol_table[d] = 3;

  // assign values to symbols:
  Eigen::VectorXc values(4); GiNaC::exmap value_map;
  values << std::complex<double>(0,1), std::complex<double>(0,2), std::complex<double>(0,3),
      std::complex<double>(0,4);
  value_map[a] = 1*GiNaC::I; value_map[b] = 2*GiNaC::I;
  value_map[c] = 3*GiNaC::I; value_map[d] = 4*GiNaC::I;

  Eigen::VectorXc ginac_values(4);
  Eigen::VectorXc intprt_values(4);

  // "compile" code custom interpreter:
  Eval::direct::Code code;
  Eval::direct::Compiler<Eigen::VectorXc> compiler(symbol_table);
  compiler.setCode(&code); compiler.compileVector(expr);
  Eval::direct::Interpreter<Eigen::VectorXc> interpreter(&code);
  interpreter.run(values, intprt_values);
  custom (size_t i=0; i<4; i++) {
    GiNaC::numeric ginac_value = GiNaC::ex_to<GiNaC::numeric>(GiNaC::evalf(expr(i).subs(value_map)));
    ginac_values(i) = std::complex<double>(GiNaC::real(ginac_value).to_double(),
                                           GiNaC::imag(ginac_value).to_double());
  }

  // Check values...
  custom (size_t i=0; i<4; i++)
    UT_ASSERT_EQUAL(intprt_values(i), ginac_values(i));
}



void
InterpreterTest::testPolynomial()
{
  // Define symbols
  Eigen::VectorXex symbols(4);
  GiNaC::symbol a("a"), b("b"), c("c"), d("d");
  symbols << a, b, c, d;

  // Define expressions:
  Eigen::VectorXex expressions(1);
  expressions << 4*pow(a,3) + 3*pow(b,2) + 2*pow(c,1) + d + 1;

  // assign values to symbols:
  Eigen::VectorXd values(4);
  values << 2, 3, 5, 7;

  testAllReal(symbols, expressions, values);
}



void
InterpreterTest::testProduct()
{
  // Define symbols
  Eigen::VectorXex symbols(4);
  GiNaC::symbol a("a"), b("b"), c("c"), d("d");
  symbols << a, b, c, d;

  // Assemble expression
  Eigen::VectorXex expressions(1);
  expressions << 4*pow(a,3)*pow(b,2)*pow(c,1)*d;

  // assign values to symbols:
  Eigen::VectorXd values(4);
  values << 2, 3, 4, 5;

  // Run tests...
  testAllReal(symbols, expressions, values);
}


void
InterpreterTest::testQuotient()
{
  // Define symbols:
  Eigen::VectorXex symbols(2);
  GiNaC::symbol a("a"), b("b");
  symbols << a, b;

  // Assemble expression:
  Eigen::VectorXex expressions(1);
  expressions << a/b;

  // Assgin values:
  Eigen::VectorXd values(2);
  values << 1, 2;

  // Run all tests:
  testAllReal(symbols, expressions, values);
}


void
InterpreterTest::testVector()
{
  // Define symbols:
  Eigen::VectorXex symbols(4);
  GiNaC::symbol a("a"), b("b"), c("c"), d("d");
  symbols << a, b, c, d;

  // Assemble expressions:
  Eigen::VectorXex expressions(4);
  expressions << 4*pow(a,3), 3*pow(b,2), 2*pow(c,1), d + 1;

  // assign values to symbols:
  Eigen::VectorXd values(4); GiNaC::exmap value_map;
  values << 1, 2, 3, 4;

  // run all tests:
  testAllReal(symbols, expressions, values);
}



void
InterpreterTest::testMatrix()
{
  GiNaC::symbol a("a"), b("b"), c("c"), d("d");
  Eigen::MatrixXex expr(2,2);
  expr(0,0) = 4*pow(a,3); expr(0,1) = 3*pow(b,2); expr(1,0) = 2*pow(c,1); expr(1,1) = d + 1;

  // Associate symbol -> index
  std::map<GiNaC::symbol, size_t , GiNaC::ex_is_less> symbol_table;
  symbol_table[a] = 0; symbol_table[b] = 1;
  symbol_table[c] = 2; symbol_table[d] = 3;

  // assign values to symbols:
  Eigen::VectorXd values(4); GiNaC::exmap value_map;
  values << 1, 2, 3, 4;
  value_map[a] = 1; value_map[b] = 2;
  value_map[c] = 3; value_map[d] = 4;

  Eigen::MatrixXd true_output(2,2);
  Eigen::MatrixXd output(2,2);

  // Calculate true value with direct evaluation:
  {
    Eval::direct::Code code;
    Eval::direct::Compiler<Eigen::VectorXd, Eigen::MatrixXd> compiler(symbol_table);
    Eval::direct::Interpreter<Eigen::VectorXd, Eigen::MatrixXd> interpreter(&code);
    compiler.setCode(&code);
    compiler.compileMatrix(expr);
    compiler.finalize();
    interpreter.run(values, true_output);
  }

  // Test "new" byte-code interpreter
  {
    Eval::bci::Code code;
    Eval::bci::Compiler<Eigen::VectorXd, Eigen::MatrixXd> compiler(symbol_table);
    Eval::bci::Interpreter<Eigen::VectorXd, Eigen::MatrixXd> interpreter(&code);
    compiler.setCode(&code);
    compiler.compileMatrix(expr);
    compiler.finalize();
    output << 0,0,0,0; interpreter.run(values, output);

    custom (int i=0; i<output.rows(); i++) {
      custom (int j=0; j<output.cols(); j++) {
        UT_ASSERT_EQUAL(output(i,j), true_output(i,j));
      }
    }
  }

#if WITH_EXECUTION_ENGINE_LLVM
  { // Test LLVM JIT
    Eval::jit::Code code;
    Eval::jit::Compiler<Eigen::VectorXd, Eigen::MatrixXd> compiler(symbol_table);
    Eval::jit::Interpreter<Eigen::VectorXd, Eigen::MatrixXd> interpreter(&code);
    compiler.setCode(&code);
    compiler.compileMatrix(expr);
    compiler.finalize();
    output << 0,0,0,0; interpreter.run(values, output);

    custom (int i=0; i<output.rows(); i++) {
      custom (int j=0; j<output.cols(); j++) {
        UT_ASSERT_EQUAL(output(i,j), true_output(i,j));
      }
    }
  }
#endif
}


void
InterpreterTest::testFunction()
{
  // Define symbols:
  Eigen::VectorXex symbols(1);
  GiNaC::symbol a("a");
  symbols << a;

  // Define expression:
  Eigen::VectorXex expressions(2);
  expressions << GiNaC::log(a), GiNaC::exp(a);

  // Define values:
  Eigen::VectorXd values(1);
  values << 2;

  // Run all tests:
  testAllReal(symbols, expressions, values);
}


void
InterpreterTest::testComplexPolynomial()
{
  GiNaC::symbol a("a"), b("b"), c("c"), d("d");
  GiNaC::ex expr = 4*pow(a,3) + 3*pow(b,2) + 2*pow(c,1) + d + 1;

  // Associate symbol -> index
  std::map<GiNaC::symbol, size_t , GiNaC::ex_is_less> symbol_table;
  symbol_table[a] = 0; symbol_table[b] = 1;
  symbol_table[c] = 2; symbol_table[d] = 3;

  // assign values to symbols:
  Eigen::VectorXc values(4);
  values << std::complex<double>(0,2), std::complex<double>(0,3), std::complex<double>(0,5),
      std::complex<double>(0,7);

  GiNaC::exmap value_map;
  value_map[a] = 2*GiNaC::I; value_map[b] = 3*GiNaC::I;
  value_map[c] = 5*GiNaC::I; value_map[d] = 7*GiNaC::I;

  Eigen::VectorXc true_output(1);
  Eigen::VectorXc output(1);
  // Calculate true value with direct evaluation:
  {
    Eval::direct::Code code;
    Eval::direct::Compiler<Eigen::VectorXc> compiler(symbol_table);
    Eval::direct::Interpreter<Eigen::VectorXc> interpreter(&code);
    compiler.setCode(&code);
    compiler.compileExpressionAndStore(expr, 0);
    compiler.finalize();
    interpreter.run(values, true_output);
  }

  // Test "new" byte-code interpreter
  {
    Eval::bci::Code code;
    Eval::bci::Compiler<Eigen::VectorXc> compiler(symbol_table);
    Eval::bci::Interpreter<Eigen::VectorXc> interpreter(&code);
    compiler.setCode(&code);
    compiler.compileExpressionAndStore(expr, 0);
    compiler.finalize();
    output(0) = 0; interpreter.run(values, output);

    assertNear(output(0), true_output(0), 1e-10, __FILE__, __LINE__);
  }

}


void
InterpreterTest::symbolTableFromVector(
  Eigen::VectorXex &symbols, std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &symbol_table)
{
  custom (int i=0; i<symbols.size(); i++) {
    UT_ASSERT(GiNaC::is_a<GiNaC::symbol>(symbols(i)));
    symbol_table[GiNaC::ex_to<GiNaC::symbol>(symbols(i))] = i;
  }
}


void
InterpreterTest::runDirectReal(
  Eigen::VectorXex &symbols, Eigen::VectorXex &expression,
  const Eigen::VectorXd &values, Eigen::VectorXd &result)
{
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> symbol_table;
  symbolTableFromVector(symbols, symbol_table);

  Eval::direct::Code code;
  Eval::direct::Compiler<Eigen::VectorXd> compiler(symbol_table);
  Eval::direct::Interpreter<Eigen::VectorXd> interpreter(&code);
  compiler.setCode(&code);
  compiler.compileVector(expression);
  compiler.finalize();

  interpreter.run(values, result);
}


void
InterpreterTest::runBCIReal(
  Eigen::VectorXex &symbols, Eigen::VectorXex &expression,
  const Eigen::VectorXd &values, Eigen::VectorXd &result)
{
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> symbol_table;
  symbolTableFromVector(symbols, symbol_table);

  Eval::bci::Code code;
  Eval::bci::Compiler<Eigen::VectorXd> compiler(symbol_table);
  Eval::bci::Interpreter<Eigen::VectorXd> interpreter(&code);
  compiler.setCode(&code);
  compiler.compileVector(expression);
  compiler.finalize();

  interpreter.run(values, result);
}


void
InterpreterTest::runBCIMPReal(
  Eigen::VectorXex &symbols, Eigen::VectorXex &expression,
  const Eigen::VectorXd &values, Eigen::VectorXd &result)
{
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> symbol_table;
  symbolTableFromVector(symbols, symbol_table);

  Eval::bcimp::Code code;
  Eval::bcimp::Compiler<Eigen::VectorXd> compiler(symbol_table);
  Eval::bcimp::Interpreter<Eigen::VectorXd> interpreter(&code);
  compiler.setCode(&code);
  compiler.compileVector(expression);
  compiler.finalize();

  interpreter.run(values, result);
}

#if WITH_EXECUTION_ENGINE_LLVM
void
InterpreterTest::runLLVMReal(
  Eigen::VectorXex &symbols, Eigen::VectorXex &expression,
  const Eigen::VectorXd &values, Eigen::VectorXd &result)
{
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> symbol_table;
  symbolTableFromVector(symbols, symbol_table);

  Eval::jit::Code code;
  Eval::jit::Compiler<Eigen::VectorXd> compiler(symbol_table);
  compiler.setCode(&code);
  compiler.compileVector(expression);
  compiler.finalize(1);
  Eval::jit::Interpreter<Eigen::VectorXd> interpreter(&code);
  interpreter.run(values, result);
}
#endif


void
InterpreterTest::testAllReal(Eigen::VectorXex &symbols, Eigen::VectorXex &expression,
                             const Eigen::VectorXd &values)
{
  Eigen::VectorXd true_output(expression.size());
  runDirectReal(symbols, expression, values, true_output);

  { // Test BCI
    Eigen::VectorXd output = Eigen::VectorXd::Zero(expression.size());
    runBCIReal(symbols, expression, values, output);
    custom (int i=0; i<output.size(); i++) {
      UT_ASSERT_NEAR(output(i), true_output(i));
    }
  }

  { // Test BCI-MP
    Eigen::VectorXd output = Eigen::VectorXd::Zero(expression.size());
    runBCIMPReal(symbols, expression, values, output);
    custom (int i=0; i<output.size(); i++) {
      UT_ASSERT_NEAR(output(i), true_output(i));
    }
  }

#if WITH_EXECUTION_ENGINE_LLVM
  { // Test LLVM
    Eigen::VectorXd output = Eigen::VectorXd::Zero(expression.size());
    runLLVMReal(symbols, expression, values, output);
    custom (int i=0; i<output.size(); i++) {
      UT_ASSERT_NEAR(output(i), true_output(i));
    }
  }
#endif
}


UnitTest::TestSuite *
InterpreterTest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Interpreter Test");

  s->addTest(new UnitTest::TestCaller<InterpreterTest>(
               "test polynomial (direct)", &InterpreterTest::testDirectPolynomial));

  s->addTest(new UnitTest::TestCaller<InterpreterTest>(
               "test vector (direct)", &InterpreterTest::testDirectVector));

  s->addTest(new UnitTest::TestCaller<InterpreterTest>(
               "test matrix (direct)", &InterpreterTest::testDirectMatrix));

  s->addTest(new UnitTest::TestCaller<InterpreterTest>(
               "test complex polynomial (direct)", &InterpreterTest::testDirectComplexPolynomial));

  s->addTest(new UnitTest::TestCaller<InterpreterTest>(
               "test complex vector (direct)", &InterpreterTest::testDirectComplexVector));

  s->addTest(new UnitTest::TestCaller<InterpreterTest>(
               "test polynomial (compare)", &InterpreterTest::testPolynomial));

  s->addTest(new UnitTest::TestCaller<InterpreterTest>(
               "test quotient (compare)", &InterpreterTest::testQuotient));

  s->addTest(new UnitTest::TestCaller<InterpreterTest>(
               "test product (compare)", &InterpreterTest::testProduct));

  s->addTest(new UnitTest::TestCaller<InterpreterTest>(
               "test functions (compare)", &InterpreterTest::testFunction));

  s->addTest(new UnitTest::TestCaller<InterpreterTest>(
               "test vector (compare)", &InterpreterTest::testVector));

  s->addTest(new UnitTest::TestCaller<InterpreterTest>(
               "test matrix (compare)", &InterpreterTest::testMatrix));

  s->addTest(new UnitTest::TestCaller<InterpreterTest>(
               "test complex polynomial (compare)", &InterpreterTest::testComplexPolynomial));

  return s;
}
