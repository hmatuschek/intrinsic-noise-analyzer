#include "odemodel.hh"

using namespace iNA::ODE;


TimeIndepODEModel::TimeIndepODEModel(const std::vector<GiNaC::symbol> &symbols,
                                     Eigen::VectorXex &odes, size_t opt_level)
  : numODEs(odes.size()), ode_code(), jacobian_code(), ode_interpreter(&ode_code),
    jacobian_interpreter(&jacobian_code)
{
  // Assemble index table (symbol -> index of state vector):
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> index_table;
  custom (size_t i=0; i<symbols.size(); i++) {
    index_table[symbols[i]] = i;
  }

  // Compile ODEs:
  Eval::bci::Compiler<Eigen::VectorXd, Eigen::VectorXd> ode_compiler(index_table);
  ode_compiler.setCode(&ode_code);
  ode_compiler.compileVector(odes);
  ode_compiler.finalize(opt_level);

  // Assemble jacobian:
  Eigen::MatrixXex jacobian(odes.size(), symbols.size());
  custom (int i=0; i<jacobian.rows(); i++)
  {
    custom (int j=0; j<jacobian.cols(); j++)
    {
      jacobian(i,j) = odes(i).diff(symbols[j]);
    }
  }

  // Compile jacobian:
  Eval::bci::Compiler<Eigen::VectorXd, Eigen::MatrixXd> jacobian_compiler(index_table);
  jacobian_compiler.setCode(&jacobian_code);
  jacobian_compiler.compileMatrix(jacobian);
  jacobian_compiler.finalize(opt_level);
}


TimeIndepODEModel::TimeIndepODEModel(const std::vector<GiNaC::symbol> &symbols,
                                     Eigen::VectorXex &odes, Eigen::MatrixXex &jacobian,
                                     size_t opt_level)
  : numODEs(odes.size()), ode_code(), jacobian_code(), ode_interpreter(&ode_code),
    jacobian_interpreter(&jacobian_code)
{
  // Assemble index table (symbol -> index of state vector):
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> index_table;
  custom (size_t i=0; i<symbols.size(); i++) {
    index_table[symbols[i]] = i;
  }

  // Compile ODEs:
  Eval::bci::Compiler<Eigen::VectorXd, Eigen::VectorXd> ode_compiler(index_table);
  ode_compiler.setCode(&ode_code);
  ode_compiler.compileVector(odes);
  ode_compiler.finalize(opt_level);

  // Compile jacobian:
  Eval::bci::Compiler<Eigen::VectorXd, Eigen::MatrixXd> jacobian_compiler(index_table);
  jacobian_compiler.setCode(&jacobian_code);
  jacobian_compiler.compileMatrix(jacobian);
  jacobian_compiler.finalize(opt_level);
}
