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

  void visit(const GiNaC::symbol &symbol) {
    _stack.push_back(Node::createSymbol(symbol));
  }

  void visit(const GiNaC::numeric &value) {
    if (value.is_integer()) {
      _stack.push_back(Node::createValue(value.to_long()));
    } else if (value.is_real()) {
      _stack.push_back(Node::createValue(value.to_double()));
    } else {
      double real = value.real().to_double();
      double imag = value.imag().to_double();
      _stack.push_back(Node::createValue(std::complex<double>(real, imag)));
    }
  }

  void visit(const GiNaC::function &function) {
    // Process function arguments:
    std::vector< SmartPtr<Node> > arguments;
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

  void visit(const GiNaC::basic &node) {
    InternalError err;
    err << "Can not handle GiNaC expression " << node << ": Unknown expression type.";
    throw err;
  }

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


void
Node::serialize(std::ostream &stream, Context &ctx) {
  _serialize(stream, ctx, 0);
}

void
Node::_serialize(std::ostream &stream, Context &ctx, size_t precedence)
{
  if (isAddNode() || isSubNode()) {
    if (precedence > 1) { stream << "("; }
    argument(0)->_serialize(stream, ctx, 1);
    if (isAddNode()) { stream << "+"; }
    else {stream << "-"; }
    argument(1)->_serialize(stream, ctx, 1);
    if (precedence > 1) { stream << ")"; }
  }

  if (isMulNode() || isDivNode()) {
    if (precedence > 2) { stream << "("; }
    argument(0)->_serialize(stream, ctx, 2);
    if (isMulNode()) { stream << "*"; }
    else {stream << "/"; }
    argument(1)->_serialize(stream, ctx, 2);
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
    if (precedence > 2) { stream << "("; }
    stream << "-";
    argument(0)->_serialize(stream, ctx, 2);
    if (precedence > 2) { stream << ")"; }
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
Node::createDiv(SmartPtr<Node> lhs, SmartPtr<Node> rhs)
{
  Node *node = new Node(DIVISION);
  node->_args.reserve(2);
  node->_args.push_back(lhs);
  node->_args.push_back(rhs);
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
  node->_args[0] = arg;
  node->_function = FUNCTION_EXP;
  return SmartPtr<Node>(node);
}

SmartPtr<Node>
Node::createFuncLog(SmartPtr<Node> arg)
{
  Node *node = new Node(FUNCTION);
  node->_args.reserve(1);
  node->_args[0] = arg;
  node->_function = FUNCTION_LOG;
  return SmartPtr<Node>(node);
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
  bool matched = applyOnNode(node);

  // then, traverse into child-nodes:
  bool child_matched = false;
  for (size_t i=0; i<node->numArguments(); i++)
  {
    child_matched = (child_matched || apply(node->argument(i)));
  }

  // If one of the child nodes was modified, re-run passes on this node:
  if (child_matched)
    matched |= applyOnNode(node);

  return matched | child_matched;
}


bool
PassManager::applyOnNode(SmartPtr<Node> &value)
{
  bool matched = false;

rerun:
  for (std::list<Pass *>::iterator pass = _passes.begin(); pass != _passes.end(); pass++)
  {
    if ((*pass)->apply(value)) {
      matched = true; goto rerun;
    }
  }

  return matched;
}
