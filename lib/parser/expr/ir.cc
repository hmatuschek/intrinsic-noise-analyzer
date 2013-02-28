#include "ir.hh"
#include <exception.hh>
#include <list>


using namespace iNA;
using namespace iNA::Parser::Expr;


/* ********************************************************************************************* *
 * Implementation of Ginac2Node, translation of GiNaC expressions to IR
 * ********************************************************************************************* */
class Ginac2Node :
    public GiNaC::visitor, public GiNaC::add::visitor, public GiNaC::mul::visitor,
    public GiNaC::power::visitor, public GiNaC::symbol::visitor, public GiNaC::numeric::visitor,
    public GiNaC::function::visitor, public GiNaC::basic::visitor
{
protected:
  /** Internal stack of expressions. */
  std::list< SmartPtr<Node> > _stack;

public:
  /** Handles sums. */
  void visit(const GiNaC::add &add) {
    // Skip empty sums:
    if (0 == add.nops()) { return; }
    // Process each summand
    for (size_t i=0; i<add.nops(); i++) { add.op(i).accept(*this); }
    // Assemble sum
    SmartPtr<Node> rhs = _stack.back(); _stack.pop_back();
    for (size_t i=1; i<add.nops(); i++) {
      SmartPtr<Node> lhs = _stack.back(); _stack.pop_back();
      rhs = Node::createAdd(lhs, rhs);
    }
    _stack.push_back(rhs);
  }

  /** Handles products. */
  void visit(const GiNaC::mul &mul) {
    // Skip empty products:
    if (0 == mul.nops()) { return; }
    // Process each factor
    for (size_t i=0; i<mul.nops(); i++) { mul.op(i).accept(*this); }
    // Assemble product
    SmartPtr<Node> rhs = _stack.back(); _stack.pop_back();
    for (size_t i=1; i<mul.nops(); i++) {
      SmartPtr<Node> lhs = _stack.back(); _stack.pop_back();
      rhs = Node::createMul(lhs, rhs);
    }
    _stack.push_back(rhs);
  }

  /** Handles powers. */
  void visit(const GiNaC::power &power) {
    // Process base
    power.op(0).accept(*this);
    SmartPtr<Node> base = _stack.back(); _stack.pop_back();
    // process exponent
    power.op(1).accept(*this);
    SmartPtr<Node> exponent = _stack.back(); _stack.pop_back();
    // Assemble power
    _stack.push_back(Node::createPow(base, exponent));
  }

  /** Handles symbols. */
  void visit(const GiNaC::symbol &symbol) {
    _stack.push_back(Node::createSymbol(symbol));
  }

  /** Handles numeric values. */
  void visit(const GiNaC::numeric &value) {
    if (value.is_integer()) {
      if (GiNaC::abs(value) > 1e6) { // big integer
        _stack.push_back(Node::createValue(value.to_double()));
      } else { // small integer
        _stack.push_back(Node::createValue(value.to_long()));
      }
    } else if (value.is_real()) {
      _stack.push_back(Node::createValue(value.to_double()));
    } else {
      double real = value.real().to_double();
      double imag = value.imag().to_double();
      _stack.push_back(Node::createValue(std::complex<double>(real, imag)));
    }
  }

  /** Handles function calls. */
  void visit(const GiNaC::function &function) {
    // Process function arguments:
    std::vector< SmartPtr<Node> > arguments(function.nops());
    for (size_t i=0; i<function.nops(); i++) {
      function.op(i).accept(*this);
      arguments[i] = _stack.back(); _stack.pop_back();
    }
    // Dispatch by function serial:
    if (GiNaC::exp_SERIAL::serial == function.get_serial()) {
      _stack.push_back(Node::createFuncExp(arguments[0])); return;
    }
    if (GiNaC::log_SERIAL::serial == function.get_serial()) {
      _stack.push_back(Node::createFuncLog(arguments[0])); return;
    }

    InternalError err;
    err << "Can not handle GiNaC function call " << function << ": Unknown function.";
    throw err;
  }

  /** Handles all unhandled expressions. */
  void visit(const GiNaC::basic &node) {
    InternalError err;
    err << "Can not handle GiNaC expression " << node << ": Unknown expression type.";
    throw err;
  }

  /** Removes and returns the top element of the stack. */
  SmartPtr<Node> popNode() {
    SmartPtr<Node> element = _stack.back(); _stack.pop_back();
    return element;
  }
};


/* ********************************************************************************************* *
 * Implementation of Expr::Node, the IR node.
 * ********************************************************************************************* */
Node::Node(NodeType type)
  : _type(type)
{
  // Pass...
}


bool Node::isAddNode() const { return ADDITION == _type; }
bool Node::isSubNode() const { return SUBTRACTION == _type; }
bool Node::isMulNode() const { return MULTIPLICATION == _type; }
bool Node::isDivNode() const { return DIVISION == _type; }
bool Node::isPowNode() const { return POWER == _type; }
bool Node::isNegNode() const { return NEGATION == _type; }
bool Node::isSymbolNode() const { return SYMBOL == _type; }
bool Node::isValueNode() const { return (VALUE_INTEGER == _type) || (VALUE_REAL == _type) ||
      (VALUE_COMPLEX == _type); }
bool Node::isIntegerNode() const { return VALUE_INTEGER == _type; }
bool Node::isRealNode() const { return VALUE_REAL == _type; }
bool Node::isComplexNode() const { return VALUE_COMPLEX == _type; }
bool Node::isFunctionNode() const { return FUNCTION == _type; }
bool Node::isFuncExpNode() const { return isFunctionNode() && (FUNCTION_EXP == _function); }
bool Node::isFuncLogNode() const { return isFunctionNode() && (FUNCTION_LOG == _function); }

bool Node::hasArguments() const { return 0 != _args.size(); }
size_t Node::numArguments() const { return _args.size(); }
SmartPtr<Node> &Node::argument(size_t i) { return _args[i]; }

long &Node::intValue() { return _integer; }
double &Node::realValue() { return _real; }
std::complex<double> &Node::complexValue() { return _complex; }
GiNaC::symbol Node::symbol() const { return _symbol; }

void
Node::serialize(std::ostream &stream, Context &ctx) {
  _serialize(stream, ctx, 0);
}

void
Node::_serialize(std::ostream &stream, Context &ctx, size_t precedence)
{
  if (isAddNode()) {
    if (precedence > 1) { stream << "("; }
    argument(0)->_serialize(stream, ctx, 1);
    stream << "+";
    argument(1)->_serialize(stream, ctx, 1);
    if (precedence > 1) { stream << ")"; }
  }

  if (isSubNode()) {
    if (precedence > 1) { stream << "("; }
    argument(0)->_serialize(stream, ctx, 1);
    stream << "-";
    argument(1)->_serialize(stream, ctx, 2);
    if (precedence > 1) { stream << ")"; }
  }

  if (isMulNode()) {
    if (precedence > 2) { stream << "("; }
    argument(0)->_serialize(stream, ctx, 2);
    stream << "*";
    argument(1)->_serialize(stream, ctx, 2);
    if (precedence > 2) { stream << ")"; }
  }

  if (isDivNode()) {
    if (precedence > 2) { stream << "("; }
    argument(0)->_serialize(stream, ctx, 2);
    stream << "/";
    argument(1)->_serialize(stream, ctx, 3);
    if (precedence > 2) { stream << ")"; }
  }

  if (isPowNode()) {
    if (precedence >= 3) { stream << "("; }
    argument(0)->_serialize(stream, ctx, 3);
    stream << "**";
    argument(1)->_serialize(stream, ctx, 3);
    if (precedence >= 3) { stream << ")"; }
  }

  if (isNegNode()) {
    if (precedence >= 2) { stream << "("; }
    stream << "-";
    argument(0)->_serialize(stream, ctx, 2);
    if (precedence >= 2) { stream << ")"; }
  }

  if (isFunctionNode()) {
    if (isFuncExpNode()) {
      stream << "exp(";
      argument(0)->_serialize(stream, ctx, 0);
      stream << ")";
    }
    if (isFuncLogNode()) {
      stream << "log(";
      argument(0)->_serialize(stream, ctx, 0);
      stream << ")";
    }
  }

  if (isSymbolNode()) {
    stream << ctx.identifier(_symbol);
  }

  if (isIntegerNode()) {
    stream << _integer;
  }
  if (isRealNode()) {
    stream << _real;
  }
  if (isComplexNode()) {
    if (precedence > 1) { stream << "("; }
    stream << _complex.real() << "+" << _complex.imag() << "j";
    if (precedence > 1) { stream << ")"; }
  }
}


SmartPtr<Node>
Node::fromExpression(GiNaC::ex expression)
{
  Ginac2Node assembler; expression.accept(assembler);
  return assembler.popNode();
}

SmartPtr<Node>
Node::createAdd(SmartPtr<Node> lhs, SmartPtr<Node> rhs)
{
  Node *node = new Node(ADDITION);
  node->_args.reserve(2);
  node->_args.push_back(lhs);
  node->_args.push_back(rhs);
  return SmartPtr<Node>(node);
}

SmartPtr<Node>
Node::createSub(SmartPtr<Node> lhs, SmartPtr<Node> rhs)
{
  Node *node = new Node(SUBTRACTION);
  node->_args.reserve(2);
  node->_args.push_back(lhs);
  node->_args.push_back(rhs);
  return SmartPtr<Node>(node);
}

SmartPtr<Node>
Node::createMul(SmartPtr<Node> lhs, SmartPtr<Node> rhs)
{
  Node *node = new Node(MULTIPLICATION);
  node->_args.reserve(2);
  node->_args.push_back(lhs);
  node->_args.push_back(rhs);
  return SmartPtr<Node>(node);
}

SmartPtr<Node>
Node::createDiv(SmartPtr<Node> nom, SmartPtr<Node> denom)
{
  Node *node = new Node(DIVISION);
  node->_args.reserve(2);
  node->_args.push_back(nom);
  node->_args.push_back(denom);
  return SmartPtr<Node>(node);
}

SmartPtr<Node>
Node::createPow(SmartPtr<Node> base, SmartPtr<Node> exponent)
{
  Node *node = new Node(POWER);
  node->_args.reserve(2);
  node->_args.push_back(base);
  node->_args.push_back(exponent);
  return SmartPtr<Node>(node);
}

SmartPtr<Node>
Node::createNeg(SmartPtr<Node> op)
{
  Node *node = new Node(NEGATION);
  node->_args.reserve(1);
  node->_args.push_back(op);
  return SmartPtr<Node>(node);
}

SmartPtr<Node>
Node::createSymbol(GiNaC::symbol symbol)
{
  Node *node = new Node(SYMBOL);
  node->_symbol = symbol;
  return SmartPtr<Node>(node);
}

SmartPtr<Node>
Node::createValue(long value)
{
  Node *node = new Node(VALUE_INTEGER);
  node->_integer = value;
  return SmartPtr<Node>(node);
}

SmartPtr<Node>
Node::createValue(const double &value)
{
  Node *node = new Node(VALUE_REAL);
  node->_real = value;
  return SmartPtr<Node>(node);
}

SmartPtr<Node>
Node::createValue(const std::complex<double> &value)
{
  Node *node = new Node(VALUE_COMPLEX);
  node->_complex = value;
  return SmartPtr<Node>(node);
}


SmartPtr<Node>
Node::createFuncExp(SmartPtr<Node> arg)
{
  Node *node = new Node(FUNCTION);
  node->_args.reserve(1);
  node->_args.push_back(arg);
  node->_function = FUNCTION_EXP;
  return SmartPtr<Node>(node);
}

SmartPtr<Node>
Node::createFuncLog(SmartPtr<Node> arg)
{
  Node *node = new Node(FUNCTION);
  node->_args.reserve(1);
  node->_args.push_back(arg);
  node->_function = FUNCTION_LOG;
  return SmartPtr<Node>(node);
}



/* ********************************************************************************************* *
 * Implementation of PassManager
 * ********************************************************************************************* */
Pass::~Pass() {
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of PassManager
 * ********************************************************************************************* */
PassManager::~PassManager()
{
  for (std::list<Pass *>::iterator pass=_passes.begin(); pass!=_passes.end(); pass++) {
    delete *pass;
  }
}

void
PassManager::addPass(Pass *pass) {
  _passes.push_back(pass);
}

bool
PassManager::apply(SmartPtr<Node> &node) {
  // First, apply passes
  bool matched = false;
  bool child_matched = false;

rerun:
  child_matched = false;
  // then, traverse into child-nodes:
  for (size_t i=0; i<node->numArguments(); i++) {
    child_matched = (child_matched || apply(node->argument(i)));
  }
  matched = applyOnNode(node);

  // If one of the child nodes was modified, re-run passes on this node:
  if (child_matched) {
    goto rerun;
  }

  return matched | child_matched;
}


bool
PassManager::applyOnNode(SmartPtr<Node> &value)
{
  bool matched = false;

rerun:
  for (std::list<Pass *>::iterator pass=_passes.begin(); pass!=_passes.end(); pass++)
  {
    if ((*pass)->apply(value)) {
      matched = true; goto rerun;
    }
  }

  return matched;
}



/* ********************************************************************************************* *
 * PASS: a^(-b) -> 1/a^b
 * ********************************************************************************************* */
bool
PowerToDivisionPass::apply(SmartPtr<Node> &node)
{
  // Skip non power expressions:
  if (! node->isPowNode()) { return false; }

  SmartPtr<Node> base = node->argument(0);
  SmartPtr<Node> exponent = node->argument(1);

  // Check if exponent is a negation:
  if (! exponent->isNegNode()) { return false; }

  // a^(-b) -> 1/a^b
  node = Node::createDiv(Node::createValue(1L), Node::createPow(base, exponent->argument(0)));
  return true;
}


/* ********************************************************************************************* *
 * PASS: 0+a, 1*a, etc.
 * ********************************************************************************************* */
bool
RemoveUnitsPass::apply(SmartPtr<Node> &node)
{
  // Dispatch by node type:
  if (node->isAddNode()) {
    return _applyOnAdd(node);
  } else if (node->isSubNode()) {
    return _applyOnSub(node);
  } else if (node->isMulNode()) {
    return _applyOnMul(node);
  } else if (node->isDivNode()) {
    return _applyOnDiv(node);
  } else if (node->isPowNode()) {
    return _applyOnPow(node);
  } else if (node->isNegNode()) {
    return _applyOnNeg(node);
  }
  return false;
}


bool
RemoveUnitsPass::_applyOnAdd(SmartPtr<Node> &node)
{
  SmartPtr<Node> lhs = node->argument(0);
  SmartPtr<Node> rhs = node->argument(1);

  if ( (lhs->isIntegerNode() && (0 == lhs->intValue())) ||
       (lhs->isRealNode() && (0 == lhs->realValue())) ||
       (lhs->isComplexNode() && (0. == lhs->complexValue())) )
  {
    node = rhs;
    return true;
  }

  if ( (rhs->isIntegerNode() && (0 == rhs->intValue())) ||
       (rhs->isRealNode() && (0 == rhs->realValue())) ||
       (rhs->isComplexNode() && (0. == rhs->complexValue())) )
  {
    node = lhs;
    return true;
  }

  return false;
}


bool
RemoveUnitsPass::_applyOnSub(SmartPtr<Node> &node)
{
  SmartPtr<Node> lhs = node->argument(0);
  SmartPtr<Node> rhs = node->argument(1);

  if ( (lhs->isIntegerNode() && (0 == lhs->intValue())) ||
       (lhs->isRealNode() && (0 == lhs->realValue())) ||
       (lhs->isComplexNode() && (0. == lhs->complexValue())) )
  {
    node = Node::createNeg(rhs);
    return true;
  }

  if ( (rhs->isIntegerNode() && (0 == rhs->intValue())) ||
       (rhs->isRealNode() && (0 == rhs->realValue())) ||
       (rhs->isComplexNode() && (0. == rhs->complexValue())) )
  {
    node = lhs;
    return true;
  }

  return false;
}


bool
RemoveUnitsPass::_applyOnMul(SmartPtr<Node> &node)
{
  SmartPtr<Node> lhs = node->argument(0);
  SmartPtr<Node> rhs = node->argument(1);

  if ( (lhs->isIntegerNode() && (0 == lhs->intValue())) ||
       (lhs->isRealNode() && (0 == lhs->realValue())) ||
       (lhs->isComplexNode() && (0. == lhs->complexValue())) )
  {
    node = Node::createValue(0L);
    return true;
  }

  if ( (rhs->isIntegerNode() && (0 == rhs->intValue())) ||
       (rhs->isRealNode() && (0 == rhs->realValue())) ||
       (rhs->isComplexNode() && (0. == rhs->complexValue())) )
  {
    node = Node::createValue(0L);
    return true;
  }

  if ( (lhs->isIntegerNode() && (1 == lhs->intValue())) ||
       (lhs->isRealNode() && (1 == lhs->realValue())) ||
       (lhs->isComplexNode() && (1. == lhs->complexValue())) )
  {
    node = rhs;
    return true;
  }

  if ( (rhs->isIntegerNode() && (1 == rhs->intValue())) ||
       (rhs->isRealNode() && (1 == rhs->realValue())) ||
       (rhs->isComplexNode() && (1. == rhs->complexValue())) )
  {
    node = lhs;
    return true;
  }

  return false;
}


bool
RemoveUnitsPass::_applyOnDiv(SmartPtr<Node> &node)
{
  SmartPtr<Node> nom = node->argument(0);
  SmartPtr<Node> denom = node->argument(1);

  if ( (denom->isIntegerNode() && (1 == denom->intValue())) ||
       (denom->isRealNode() && (1 == denom->realValue())) ||
       (denom->isComplexNode() && (1. == denom->complexValue())) )
  {
    node = nom;
    return true;
  }

  return false;
}


bool
RemoveUnitsPass::_applyOnPow(SmartPtr<Node> &node)
{
  SmartPtr<Node> base = node->argument(0);
  SmartPtr<Node> exponent = node->argument(1);

  if ( (exponent->isIntegerNode() && (0 == exponent->intValue())) ||
       (exponent->isRealNode() && (0 == exponent->realValue())) ||
       (exponent->isComplexNode() && (0. == exponent->complexValue())) )
  {
    node = Node::createValue(1L);
    return true;
  }

  if ( (exponent->isIntegerNode() && (1 == exponent->intValue())) ||
       (exponent->isRealNode() && (1 == exponent->realValue())) ||
       (exponent->isComplexNode() && (1. == exponent->complexValue())) )
  {
    node = base;
    return true;
  }

  return false;
}


bool
RemoveUnitsPass::_applyOnNeg(SmartPtr<Node> &node)
{
  SmartPtr<Node> arg = node->argument(0);

  // -(-(a)) -> a
  if (arg->isNegNode()) {
    // looks wired but necessary to avoid direct setting of references, which breakes
    // reference counter.
    arg = arg->argument(0);
    node = arg;
  }

  return false;
}



/* ********************************************************************************************* *
 * PASS: a+(-b) -> a-b; a-(-b) -> a+b; -a-b -> -(a+b); etc
 * ********************************************************************************************* */
bool
NormalizeOperatorPass::apply(SmartPtr<Node> &node)
{
  // Dispatch
  if (node->isAddNode()) {
    return _applyOnAdd(node);
  } else if (node->isSubNode()) {
    return _applyOnSub(node);
  } else if (node->isMulNode()) {
    return _applyOnMul(node);
  } else if (node->isDivNode()) {
    return _applyOnDiv(node);
  }

  return false;
}


bool
NormalizeOperatorPass::_applyOnAdd(SmartPtr<Node> &node)
{
  SmartPtr<Node> lhs = node->argument(0);
  SmartPtr<Node> rhs = node->argument(1);

  // (-a)+b -> b-a
  if (lhs->isNegNode()) {
    node = Node::createSub(rhs, lhs->argument(0));
    return true;
  }

  // a+(-b) -> a-b
  if (rhs->isNegNode()) {
    node = Node::createSub(lhs, rhs->argument(0));
    return true;
  }

  return false;
}


bool
NormalizeOperatorPass::_applyOnSub(SmartPtr<Node> &node)
{
  SmartPtr<Node> lhs = node->argument(0);
  SmartPtr<Node> rhs = node->argument(1);

  // a-(-b) -> a+b
  if (rhs->isNegNode()) {
    node = Node::createAdd(lhs, rhs->argument(0));
    return true;
  }

  // -a-b -> -(a+b)
  if (lhs->isNegNode()) {
    node = Node::createNeg(Node::createAdd(rhs, lhs->argument(0)));
    return true;
  }


  return false;
}


bool
NormalizeOperatorPass::_applyOnMul(SmartPtr<Node> &node)
{
  SmartPtr<Node> lhs = node->argument(0);
  SmartPtr<Node> rhs = node->argument(1);

  // -a*b -> -(a*b)
  if (lhs->isNegNode()) {
    node = Node::createNeg(Node::createMul(lhs->argument(0), rhs));
    return true;
  }

  // a*(-b) -> -(a*b)
  if (rhs->isNegNode()) {
    node = Node::createNeg(Node::createMul(lhs, rhs->argument(0)));
    return true;
  }

  // a*(b/c) -> (a*b)/c iff a is not a quotient:
  if ((!lhs->isDivNode()) && rhs->isDivNode()) {
    SmartPtr<Node> nom = rhs->argument(0);
    SmartPtr<Node> denom = rhs->argument(1);
    node = Node::createDiv(Node::createMul(lhs, nom), denom);
    return true;
  }

  // (a/b)*c -> (a*c)/b iff a is not a quotient:
  if (lhs->isDivNode() && (!rhs->isDivNode())) {
    SmartPtr<Node> nom = lhs->argument(0);
    SmartPtr<Node> denom = lhs->argument(1);
    node = Node::createDiv(Node::createMul(rhs, nom), denom);
    return true;
  }

  // (a/b)*(c/d) -> (a*c)/(b*d)
  if (lhs->isDivNode() && rhs->isDivNode()) {
    SmartPtr<Node> a = lhs->argument(0);
    SmartPtr<Node> b = lhs->argument(1);
    SmartPtr<Node> c = rhs->argument(0);
    SmartPtr<Node> d = rhs->argument(1);
    node = Node::createDiv(Node::createMul(a,c), Node::createMul(b,d));
    return true;
  }

  return false;
}


bool
NormalizeOperatorPass::_applyOnDiv(SmartPtr<Node> &node)
{
  SmartPtr<Node> nom = node->argument(0);
  SmartPtr<Node> denom = node->argument(1);

  // (-a)/b -> -(a/b)
  if (nom->isNegNode()) {
    node = Node::createNeg(Node::createDiv(nom->argument(0), denom));
    return true;
  }

  // a/(-b) -> -(a/b)
  if (denom->isNegNode()) {
    node = Node::createNeg(Node::createDiv(nom, denom->argument(0)));
    return true;
  }

  return false;
}



/* ********************************************************************************************* *
 * PASS: -1 -> -(1), 1.0 -> 1 etc...
 * ********************************************************************************************* */
bool
NormalizeValuesPass::apply(SmartPtr<Node> &node)
{
  // -1 -> -(1)
  if (node->isIntegerNode() && (0 > node->intValue())) {
    node = Node::createNeg(Node::createValue(-node->intValue()));
    return true;
  }

  // -1.0 -> -(1.0)
  if (node->isRealNode() && (0. > node->realValue())) {
    node = Node::createNeg(Node::createValue(-node->realValue()));
    return true;
  }

  // 1.0 -> 1
  if (node->isRealNode() && (node->realValue() == double(long(node->realValue())))) {
    node = Node::createValue(long(node->realValue()));
    return true;
  }

  // 1.0 + 0.0j -> 1.0
  if (node->isComplexNode() && (0. == node->complexValue().imag())) {
    node = Node::createValue(node->complexValue().real());
    return true;
  }

  return false;
}



/* ********************************************************************************************* *
 * Implementation of PrettySerializationTrafo
 * ********************************************************************************************* */
SmartPtr<Node> &
PrettySerializationTrafo::apply(SmartPtr<Node> &expression)
{
  PassManager manager;

  // Normalize values like 1.0 -> 1 etc.
  manager.addPass(new NormalizeValuesPass());
  // 1*a -> a etc...
  manager.addPass(new RemoveUnitsPass());
  // a+(-b) -> a-b etc...
  manager.addPass(new NormalizeOperatorPass());
  // a^-1 -> 1/a etc...
  manager.addPass(new PowerToDivisionPass());

  // Apply passes on expression:
  manager.apply(expression);
  return expression;
}
