#include "production.hh"
#include <stdarg.h>

using namespace Fluc::Parser;



/* ******************************************************************************************** *
 * Implementation of ConcreteSyntaxTree:
 * ******************************************************************************************** */
ConcreteSyntaxTree::ConcreteSyntaxTree()
  : type(EMPTY_NODE)
{
  // Pass...
}


ConcreteSyntaxTree::Type
ConcreteSyntaxTree::getType() const
{
  return this->type;
}

bool
ConcreteSyntaxTree::isEmpty() const
{
  return ConcreteSyntaxTree::EMPTY_NODE == this->type;
}


size_t
ConcreteSyntaxTree::getTokenIdx() const
{
  return this->index;
}


size_t
ConcreteSyntaxTree::getAltIdx() const
{
  return this->index;
}


void
ConcreteSyntaxTree::setAltIdx(size_t idx)
{
  this->index = idx;
}


bool
ConcreteSyntaxTree::matched() const
{
  return 0 != this->index;
}


void
ConcreteSyntaxTree::setMatched(bool matched)
{
  if (matched) {
    this->index = 1;
  } else {
    this->index = 0;
    ConcreteSyntaxTree::asEmptyNode(this->children[0]);
  }
}


ConcreteSyntaxTree &
ConcreteSyntaxTree::operator* ()
{
  return this->children[0];
}


ConcreteSyntaxTree &
ConcreteSyntaxTree::operator[] (size_t idx)
{
  return this->children[idx];
}


void
ConcreteSyntaxTree::asEmptyNode(ConcreteSyntaxTree &node)
{
  node.type = EMPTY_NODE;
  node.children.resize(0);
  node.index = 0;
}

void
ConcreteSyntaxTree::asTokenNode(ConcreteSyntaxTree &node, size_t index)
{
  node.type = TOKEN_NODE;
  node.children.resize(0);
  node.index = index;
}

void
ConcreteSyntaxTree::asProdNode(ConcreteSyntaxTree &node, size_t n_children)
{
  node.type = PRODUCTION_NODE;
  node.children.resize(n_children);
  node.index = 0;
}

void
ConcreteSyntaxTree::asAltNode(ConcreteSyntaxTree &node)
{
  node.type = ALTERNATIVE_NODE;
  node.children.resize(1);
  node.index = 0;
}

void
ConcreteSyntaxTree::asOptNode(ConcreteSyntaxTree &node)
{
  node.type = ALTERNATIVE_NODE;
  node.children.resize(1);
  node.index = 0;
}



/* ******************************************************************************************** *
 * Implementation of Production:
 * ******************************************************************************************** */
Production::Production()
  : elements()
{
  // Pass...
}


Production::Production(size_t num_prod, ...)
  : elements()
{
  va_list args; va_start(args, num_prod);
  for (size_t i=0; i<num_prod; i++)
  {
    this->elements.push_back(va_arg(args, Production *));
  }
  va_end(args);
}


Production::Production(const std::list<Production *> &elements)
  : elements(elements)
{
  // Pass...
}


Production::~Production()
{
  // Pass...
}


void
Production::parse(Lexer &lexer, ConcreteSyntaxTree &element)
{
  // Allocate space for all child-productions:
  ConcreteSyntaxTree::asProdNode(element, this->elements.size());

  std::list<Production *>::iterator iter=this->elements.begin();
  for (size_t i=0; i<this->elements.size(); i++, iter++)
  {
    // Try to parse production
    (*iter)->parse(lexer, element[i]);
  }

  // ok, done!
}


void
Production::notify(Lexer &lexer, ConcreteSyntaxTree &element)
{
  std::list<Production *>::iterator iter = this->elements.begin();
  for (size_t i=0; i<this->elements.size(); i++, iter++)
  {
    (*iter)->notify(lexer, element[i]);
  }
}



/* ******************************************************************************************** *
 * Implementation of TokenProduction:
 * ******************************************************************************************** */
TokenProduction::TokenProduction(unsigned id)
  : Production(), id(id)
{
  // Pass...
}


void
TokenProduction::parse(Lexer &lexer, ConcreteSyntaxTree &element)
{
  if (this->id != lexer.current().getId()) {
    SyntaxError err(lexer.current().getLine());
    err << "@line "<< lexer.current().getLine() << ": "
        << "Unexpected token: " << lexer.getTokenName(lexer.current().getId())
        << " expected: " << lexer.getTokenName(this->id);
    throw err;
  }

  ConcreteSyntaxTree::asTokenNode(element, lexer.currentIndex());

  /*std::cerr << "Parsed token " << lexer.current().getId()
            << " @line " << lexer.current().getLine()
            << ": \"" << lexer.current().getValue() << "\"" << std::endl;*/

  // Consume token...
  lexer.next();
}


void
TokenProduction::notify(Lexer &lexer, ConcreteSyntaxTree &element)
{
  // Pass...
}



/* ******************************************************************************************** *
 * Implementation of AltProduction:
 * ******************************************************************************************** */
AltProduction::AltProduction()
  : alternatives(0)
{
  // Pass...
}


AltProduction::AltProduction(size_t num_prod, ...)
  : alternatives(num_prod)
{
  va_list args; va_start(args, num_prod);
  for (size_t i=0; i<num_prod; i++)
  {
    alternatives[i] = va_arg(args, Production *);
  }
  va_end(args);
}


AltProduction::AltProduction(const std::vector<Production *> &alternatives)
  : alternatives(alternatives)
{
  // pass...
}


void
AltProduction::parse(Lexer &lexer, ConcreteSyntaxTree &element)
{
  ConcreteSyntaxTree::asAltNode(element);

  for (size_t i=0; i<this->alternatives.size(); i++)
  {
    try
    {
      // Try to parse this alternative
      lexer.push_state();
      this->alternatives[i]->parse(lexer, element[0]);
      lexer.drop_state();
      // On success store index of alternative
      element.setAltIdx(i);
      // And return
      return;
    }
    catch (ParserError &err) {
      // On error, reset lexer
      lexer.restore_state();

      // If this was the last try -> abort no alternative matched
      if ((i+1) == this->alternatives.size()) {
        throw err;
      }
    }
  }
}


void
AltProduction::notify(Lexer &lexer, ConcreteSyntaxTree &element)
{
  this->alternatives[element.getAltIdx()]->notify(lexer, element[0]);
}



/* ******************************************************************************************** *
 * Implementation of EmptyProduction:
 * ******************************************************************************************** */
EmptyProduction::EmptyProduction()
  : Production()
{
  // Pass...
}

void
EmptyProduction::parse(Lexer &lexer, ConcreteSyntaxTree &element)
{
  // Initialize element as empty:
  ConcreteSyntaxTree::asEmptyNode(element);
}

void
EmptyProduction::notify(Lexer &lexer, ConcreteSyntaxTree &element)
{
  // Pass...
}



/* ******************************************************************************************** *
 * Implementation of OptionalProduction:
 * ******************************************************************************************** */
OptionalProduction::OptionalProduction(Production *prod)
  : Production()
{
  this->production = prod;
}


void
OptionalProduction::parse(Lexer &lexer, ConcreteSyntaxTree &element)
{
  ConcreteSyntaxTree::asOptNode(element);

  // First try to parse child-production:
  lexer.push_state();
  try {
    this->production->parse(lexer, element[0]);
    lexer.drop_state();
    element.setMatched(true);
  } catch (ParserError &err) {
    lexer.restore_state();
    element.setMatched(false);
  }
}


void
OptionalProduction::notify(Lexer &lexer, ConcreteSyntaxTree &element)
{
  if (element.matched()) {
    this->production->notify(lexer, element[0]);
  }
}
