#include "ll1.hh"

using namespace iNA::Parser::LL1;



/* ******************************************************************************************** *
 * Implementation of AbstractProduction, the base of all LL(1) productions.
 * ******************************************************************************************** */
AbstractProduction::AbstractProduction()
  : _leading_token()
{
  // Pass...
}

bool
AbstractProduction::startsWith(unsigned int token) const {
  return 0 != _leading_token.count(token);
}
