#include "code.hh"

using namespace Fluc::Eval::direct;


Code::Code(size_t num_threads)
{
  // Pass...
}


Code::Code(const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &index_table)
  : index_table(index_table)
{
  // Pass...
}


Code::Code(const Code &other)
  : index_table(other.index_table), expressions(other.expressions)
{
  // Pass...
}


void
Code::addExpression(const GiNaC::ex &expression, size_t index)
{
  this->expressions.push_back(std::pair<GiNaC::ex, size_t>(expression, index));
}


Code::iterator
Code::begin()
{
  return this->expressions.begin();
}


Code::iterator
Code::end()
{
  return this->expressions.end();
}


Code::IndexTable &
Code::getIndexTable()
{
  return this->index_table;
}


void
Code::setIndexTable(const IndexTable &index_table)
{
  this->index_table = index_table;
}
