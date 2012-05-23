#ifndef INTERPRETERTEST_HH
#define INTERPRETERTEST_HH

#include "unittest.hh"
#include "fluctuator.hh"


namespace Fluc {


/**
 * Implements some simple tests for the byte-code interpreter.
 */
class InterpreterTest : public UnitTest::TestCase
{
public:
  void testDirectPolynomial();
  void testDirectVector();
  void testDirectMatrix();
  void testDirectComplexPolynomial();
  void testDirectComplexVector();

  void testPolynomial();
  void testProduct();
  void testQuotient();
  void testVector();
  void testMatrix();
  void testFunction();

  void testComplexPolynomial();
  void testComplexProduct();
  void testComplexQuotient();
  void testComplexPow();
  void testComplexVector();
  void testComplexMatrix();
  void testComplexFunction();

protected:
  /* Helper function to assemble a symbol table from a vector of symbols. */
  void symbolTableFromVector(
    Eigen::VectorXex &symbols, std::map<GiNaC::symbol, size_t , GiNaC::ex_is_less> &symbol_table);

  /* Compiles and runs a vector of expressions using Fluc::Evaluate::direct interpreter. */
  void runDirectReal(
    Eigen::VectorXex &symbols, Eigen::VectorXex &expression, const Eigen::VectorXd &values,
    Eigen::VectorXd &result);

  /* Compiles and runs a vector of expressions using Fluc::Eval default interpreter. */
  void runDefaultReal(
    Eigen::VectorXex &symbols, Eigen::VectorXex &expression, const Eigen::VectorXd &values,
    Eigen::VectorXd &result);

  /* Compiles and runs a vector of expressions using Fluc::Evaluate::bci interpreter. */
  void runBCIReal(
    Eigen::VectorXex &symbols, Eigen::VectorXex &expression, const Eigen::VectorXd &values,
    Eigen::VectorXd &result);

  /* Compiles and runs a vector of expressions using Fluc::Evaluate::bcimp interpreter. */
  void runBCIMPReal(
    Eigen::VectorXex &symbols, Eigen::VectorXex &expression, const Eigen::VectorXd &values,
    Eigen::VectorXd &result);

#if WITH_EXECUTION_ENGINE_LLVM
  /* Compiles and runs a vector of expressions using Fluc::Evaluate::LLVM interpreter. */
  void runLLVMReal(
    Eigen::VectorXex &symbols, Eigen::VectorXex &expression, const Eigen::VectorXd &values,
    Eigen::VectorXd &result);
#endif

  /* Tests if all execution-engines get the same values. */
  void testAllReal(Eigen::VectorXex &symbols, Eigen::VectorXex &expression,
                   const Eigen::VectorXd &values);

public:
  static UnitTest::TestSuite *suite();
};


}

#endif // INTERPRETERTEST_HH
