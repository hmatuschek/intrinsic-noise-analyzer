#include "consistencycheck.hh"

#include <exception.hh>
#include "constantfolder.hh"
#include "substitution.hh"

using namespace iNA;
using namespace iNA::Trafo;


void
ConsistencyCheck::assertConsistent(Ast::Model *model) throw (SemanticError)
{
  // Collect initial values of ALL variable definitions of the model:
  Substitution substitutions;
  SubstitutionCollector::collect(model, substitutions, Filter::ALL);
  // check if these substitutions are cyclic, this method throws a SemanticError exception
  // if there are any cyclic dependencies in the substitution table
  substitutions.normalize();
}


bool
ConsistencyCheck::check(Ast::Model *model)
{
  try { assert(model); }
  catch (SemanticError &err) { return false; }
  return true;
}
