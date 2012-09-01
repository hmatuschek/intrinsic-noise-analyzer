#include "dump.hh"

using namespace iNA::Ast;
using namespace iNA::Ast::Trafo;



Dump::Dump(std::ostream &stream)
  : stream(stream), indent(0)
{
  // Done.
}


void
Dump::doIndent()
{
  for (size_t i=0; i<this->indent; i++)
  {
    this->stream << "  ";
  }
}


void
Dump::dump(GiNaC::ex node, std::ostream &stream)
{
  Dump dumper(stream);
  dumper.handleExpression(node);
}


void
Dump::handleModule(Module *node)
{
  this->stream << "module {" << std::endl;
  this->indent++;

  for (Module::iterator iter = node->begin(); iter != node->end(); iter++)
  {
    this->doIndent();
    this->handleDefinition(*iter);
    this->stream << std::endl;
  }

  this->indent--;
  this->stream << "}" << std::endl;
}


void
Dump::handleVariableDefinition(VariableDefinition *node)
{
  if (node->isConst())
  {
    this->stream << "const ";
  }
  else
  {
    this->stream << "var ";
  }

  this->stream << node->getIdentifier();

  if (node->hasValue())
  {
    this->stream << " = ";
    this->handleExpression(node->getValue());
  }
}


void
Dump::handleFunctionDefinition(FunctionDefinition *node)
{
  this->stream << node->getIdentifier() << "(";
  if (0 < node->getNumArgs())
  {
    for (size_t i=0; i<node->getNumArgs()-1; i++)
    {
      this->stream << node->getArgByIdx(i);
      this->stream << ", ";
    }

    this->stream << node->getArgByIdx(node->getNumArgs()-1);
  }
  this->stream << ") => { ";

  this->handleExpression(node->getBody());

  this->stream << "}";
}


void
Dump::handleUnitDefinition(Ast::UnitDefinition *node)
{
  this->stream << "unit ";
  node->dump(this->stream);
  this->stream << std::endl;
}


void
Dump::handleReaction(Reaction *node)
{
  this->stream << "reaction " << node->getIdentifier() << " {" << std::endl;
  this->indent++;

  this->doIndent(); this->stream << "{ ";
  for (Reaction::iterator iter=node->reacBegin(); iter != node->reacEnd(); iter++)
  {
    this->stream << iter->first;
    this->stream << " @ ";
    this->handleExpression(iter->second);
    this->stream << ", ";
  }
  this->stream << "} => {";

  for (Reaction::iterator iter=node->prodBegin(); iter != node->prodEnd(); iter++)
  {
    this->stream << iter->first;
    this->stream << " @ ";
    this->handleExpression(iter->second);
    this->stream << ", ";
  }
  this->stream << "}" << std::endl;

  this->handleKineticLaw(node->getKineticLaw());
  this->indent--;
}


void
Dump::handleKineticLaw(KineticLaw *node)
{
  this->doIndent(); this->stream << "with" <<  " {" << std::endl;
  this->indent++;

  for (Scope::iterator iter=node->begin(); iter != node->end(); iter++)
  {
    this->doIndent();
    this->handleDefinition(*iter);
    this->stream << std::endl;
  }

  this->doIndent();
  this->handleExpression(node->getRateLaw());

  this->indent--;
  this->stream << std::endl; this->doIndent(); this->stream << "}";
}


void
Dump::handleExpression(GiNaC::ex node)
{
  // First, process the expression:
  this->stream << node;
}
