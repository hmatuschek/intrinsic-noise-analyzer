#ifndef __TEST_BENCHMARK_HH__
#define __TEST_BENCHMARK_HH__

#include "unittest.hh"
#include "models/IOSmodel.hh"

namespace Fluc {

class Benchmark : public UnitTest::TestCase
{
protected:
  Ast::Model *sbml_model;
  Models::IOSmodel *lna;

  void integrate_BCI_LSODA(Models::IOSmodel *model,
                           double t, size_t opt_level);

  void integrate_BCIMP_LSODA(Models::IOSmodel *model,
                             double t, size_t opt_level);

  void integrate_JIT_LSODA(Models::IOSmodel *model,
                           double t, size_t opt_level);

  void integrate_GiNaC_LSODA(Models::IOSmodel *model,
                             double t, size_t opt_level);

  void integrate_BCI_Rosen4(Models::IOSmodel *model,
                           double t, size_t opt_level);

  void integrate_BCIMP_Rosen4(Models::IOSmodel *model,
                             double t, size_t opt_level);

  void integrate_JIT_Rosen4(Models::IOSmodel *model,
                           double t, size_t opt_level);

  void simulate_BCI_gillespie(Ast::Model *model, double t, size_t opt_level);
  void simulate_JIT_gillespie(Ast::Model *model, double t, size_t opt_level);
  void simulate_GiNaC_gillespie(Ast::Model *model, double t, size_t opt_level);
  void simulate_BCI_optSSA(Ast::Model *model, double t, size_t opt_level);
  void simulate_JIT_optSSA(Ast::Model *model, double t, size_t opt_level);
  void simulate_GiNaC_optSSA(Ast::Model *model, double t, size_t opt_level);

protected:
  static size_t N_steps;
  static double eps_abs;
  static double eps_rel;
  static double t_end;
  static size_t ensemble_size;

public:
  void setUp();
  void tearDown();

  void testCoremodelBCILSODAOpt();
  void testCoremodelBCIMPLSODAOpt();
  void testCoremodelJITLSODAOpt();
  void testCoremodelGiNaCLSODA();

  void testCoremodelBCILSODANoOpt();
  void testCoremodelBCIMPLSODANoOpt();
  void testCoremodelJITLSODANoOpt();

  void testCoremodelBCIRosen4Opt();
  void testCoremodelBCIMPRosen4Opt();
  void testCoremodelJITRosen4Opt();
  void testCoremodelBCIRosen4NoOpt();
  void testCoremodelBCIMPRosen4NoOpt();
  void testCoremodelJITRosen4NoOpt();

  void testCoremodelBCIGillespieOpt();
  void testCoremodelBCIGillespieNoOpt();
  void testCoremodelJITGillespieOpt();
  void testCoremodelJITGillespieNoOpt();
  void testCoremodelGiNaCGillespie();

  void testCoremodelBCIOptSSAOpt();
  void testCoremodelBCIOptSSANoOpt();
  void testCoremodelJITOptSSAOpt();
  void testCoremodelJITOptSSANoOpt();
  void testCoremodelGiNaCOptSSA();

public:
  static UnitTest::TestSuite *suite();
};

}


#endif // BENCHMARK_HH
