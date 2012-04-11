#include "lnainterpreter.hh"

using namespace Fluc::Models;


LNAinterpreter::LNAinterpreter(LinearNoiseApproximation &model, size_t opt_level,
                               size_t num_threads, bool compileJac)
  : lnaModel(model), bytecode(num_threads), jacobianCode(num_threads),
    hasJacobian(false), opt_level(opt_level)
{
  // Compile expressions
  Evaluate::bcimp::Compiler<Eigen::VectorXd> compiler(model.stateIndex);
  compiler.setCode(&this->bytecode);
  compiler.compileVector(lnaModel.updateVector);
  compiler.finalize(opt_level);

  // Set bytecode for interpreter
  this->interpreter.setCode(&(this->bytecode));
  this->jacobian_interpreter.setCode(&(this->jacobianCode));

  if (compileJac)
    this->compileJacobian();
}


void
LNAinterpreter::compileJacobian()
{
  if(hasJacobian) return;

  // Assemble Jacobian
  Eigen::MatrixXex jacobian(lnaModel.getDimension(), lnaModel.getDimension());
  {
    std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less>::const_iterator it;
    for(it = lnaModel.stateIndex.begin(); it != lnaModel.stateIndex.end(); ++it)
    {
      for (size_t i=0; i<lnaModel.getDimension(); i++)
        jacobian(i,(*it).second) = lnaModel.updateVector(i).diff((*it).first);
    }
  }

  // Compile jacobian:
  Evaluate::bcimp::Compiler<Eigen::VectorXd, Eigen::MatrixXd> jacobian_compiler(lnaModel.stateIndex);
  jacobian_compiler.setCode(&jacobianCode);
  jacobian_compiler.compileMatrix(jacobian);
  jacobian_compiler.finalize(opt_level);

  hasJacobian = true;
}


void
LNAinterpreter::full_state(const Eigen::VectorXd &state, Eigen::VectorXd &concentrations,
                Eigen::MatrixXd &covariance, Eigen::VectorXd &emre)
{
  this->lnaModel.fullState(state,concentrations,covariance,emre);
}


void
LNAinterpreter::getInitialState(Eigen::VectorXd &state)
{
  this->lnaModel.getInitialState(state);
}

size_t
LNAinterpreter::getDimension()
{
  return this->lnaModel.getDimension();
}



