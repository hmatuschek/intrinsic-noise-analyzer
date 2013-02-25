#include "production.hh"
#include <stdarg.h>

using namespace iNA::Parser;



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
ConcreteSyntaxTree::addChild() {
  this->children.push_back(ConcreteSyntaxTree());
}

void
ConcreteSyntaxTree::removeChild() {
  this->children.pop_back();
}

size_t
ConcreteSyntaxTree::size() const {
  return this->children.size();
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

void
ConcreteSyntaxTree::asListNode(ConcreteSyntaxTree &node)
{
  node.type = LIST_NODE;
  node.children.clear();
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



/* ******************************************************************************************** *
 * Implementation of TokenProduction:
 * ******************************************************************************************** */
TokenProduction::TokenProduction(unsigned id, bool is_terminal)
  : Production(), _id(id), _is_terminal(is_terminal)
{
  // Pass...
}


void
TokenProduction::parse(Lexer &lexer, ConcreteSyntaxTree &element)
{
  // Check if the current token of the lexer matches token _id.
  if (_id != lexer.current().getId()) {
    SyntaxError err(lexer.current().getLine());
    err << "@line "<< lexer.current().getLine() << ": "
        << "Unexpected token: " << lexer.getTokenName(lexer.current().getId())
        << " expected: " << lexer.getTokenName(_id);
    err.addExpectedTokenId(lexer.getTokenName(_id));
    throw err;
  }

  // If matches -> initialize element as token node.
  ConcreteSyntaxTree::asTokenNode(element, lexer.currentIndex());

  // Consume token...
  lexer.setTerminal(_is_terminal);
  lexer.next();
}


bool
TokenProduction::isTerminal() const {
  return _is_terminal;
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

  SyntaxError coll_err(lexer.current().getLine());
  coll_err << "Unexpected token " << lexer.getTokenName(lexer.current().getId()) << ".";

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
    catch (SyntaxError &err) {
      coll_err.addExpectedTokens(err.getExpectedTokens());

      // If the lexer state is terminal, forward error
      if (lexer.isTerminal()) {
        throw err;
      }

      // On error, restore lexer state
      lexer.restore_state();

      // If this was the last try -> abort no alternative matched
      if ((i+1) == this->alternatives.size()) {
        coll_err << "expected one of: ";
        for (std::set<std::string>::const_iterator iter=coll_err.getExpectedTokens().begin();
             iter != coll_err.getExpectedTokens().end(); iter++) {
          err << *iter << ", ";
        }
        throw err;
      }
    }
  }
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
  } catch (SyntaxError &err) {
    // otherwise ignore error;
    lexer.restore_state();
    element.setMatched(false);
  }
}


/* ******************************************************************************************** *
 * Implementation of ListProduction:
 * ******************************************************************************************** */
ListProduction::ListProduction(Production *production)
  : Production(), _production(production)
{
  // pass...
}

void
ListProduction::parse(Lexer &lexer, ConcreteSyntaxTree &element)
{
  ConcreteSyntaxTree::asListNode(element);

  // Try to parse as much as possible list elements:
  size_t idx = 0;
  while (true) {
    lexer.push_state();
    try {
      element.addChild();
      this->_production->parse(lexer, element[idx]);
      lexer.drop_state(); idx++;
    } catch (SyntaxError &err) {
      lexer.restore_state();
      element.removeChild();
      return;
    }
  }
}
