#include "evaluate.hh"
#include "exception.hh"

#include "trafo/substitutioncollector.hh"

#include <cmath>

using namespace Fluc::Ast;



/* ********************************************************************************************* *
 * Implementation Evaluate class:
 * ********************************************************************************************* */
Evaluate::Evaluate()
  : value_table()
{
  // Pass...
}


Evaluate::Evaluate(const GiNaC::exmap &table)
  : value_table(table)
{
  // Pass...
}


void
Evaluate::setValue(const GiNaC::symbol &sy, const GiNaC::ex &val)
{
  // Strore substitution symbol -> reduced value:
  this->value_table[sy] = val.subs(this->value_table);
}


double
Evaluate::evaluate(const GiNaC::ex &expr)
{
  GiNaC::ex temp_value;

  try {
    // First, evaluate the ginac expression
    temp_value = GiNaC::evalf(expr.subs(this->value_table));
  } catch (std::runtime_error err) {
    NumericError new_err;
    new_err << "Cannot evaluate expression " << expr
            << ": A runtime exception was thrown: " << err.what();
    throw new_err;
  } catch (std::exception err) {
    InternalError new_err;
    new_err << "Cannot evaluate expression " << expr
            << " : An unhandled exception was thrown: " << err.what();
    throw new_err;
  }

  // Check if it was reduced to a single number:
  if (! GiNaC::is_a<GiNaC::numeric>(temp_value))
  {
    SymbolError err;
    err << "Cannot evaluate expression: " << expr << ": It is not reduced to a value!"
        << " Minimal expression: " << temp_value;
    throw err;
  }

  return GiNaC::ex_to<GiNaC::numeric>(temp_value).to_double();
}




/* ********************************************************************************************* *
 * Implementation EvaluateInitialValue class:
 * ********************************************************************************************* */
EvaluateInitialValue::EvaluateInitialValue(Module &module)
  : Evaluate()
{
  // Collect all constants and populate the value_table
  Trafo::SubstitutionCollector collector(this->value_table);
  collector.handleModule(&module);

  // Collect all initial values for all variables in given module.
  this->handleModule(&module);
}


void
EvaluateInitialValue::handleVariableDefinition(Ast::VariableDefinition *node)
{
  // If variable has no initial value -> skip
  if (! node->hasValue())
  {
    // Just traverse further:
    Walker::handleVariableDefinition(node);
  }

  // Otherwise -> associate initial value with symbol of variable.
  this->setValue(node->getSymbol(), node->getValue());

  Walker::handleVariableDefinition(node);
}




/* ********************************************************************************************* *
 * Implementation EvaluateByIndex class:
 * ********************************************************************************************* */
EvaluateByIndex::EvaluateByIndex(const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &index_table)
  : Evaluate(), index_table(index_table)
{
  // Pass...
}


void
EvaluateByIndex::setValues(const Eigen::VectorXd &values)
{
  // Iterate over all symbols, that have some index assigned:
  for (std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less>::iterator iter = this->index_table.begin();
       iter != this->index_table.end(); iter++)
  {
    // and associate the symbol with the value in values at its index:
    size_t idx = iter->second;
    double val = values(idx);
    this->setValue(iter->first, val);
  }

  // Done.
}



/* ********************************************************************************************* *
 * Implementation EvaluateModel class:
 * ********************************************************************************************* */
EvaluateModel::EvaluateModel(Ast::Model &model)
  : Evaluate(), model(model)
{
  // Pass...
}


void
EvaluateModel::setValues(const Eigen::VectorXd &values)
{
  // Iterate over the indeces of species and assign the value to it:
  for (size_t i=0; i<this->model.numSpecies(); i++)
  {
    try {
      this->setValue(this->model.getSpecies(i)->getSymbol(), GiNaC::numeric(values(i)));
    } catch (std::runtime_error &rt_err) {
      RuntimeError err;
      err << "Can not convert value " << values(i)
          << " into a ginac expression: " << rt_err.what();
      throw err;
    }
  }

  // Done.
}
