#include "astnode.hh"

using namespace iNA::Parser::Expr2;

AstNode::AstNode(long value)
  : _type(TYPE_INTEGER), _integer(value), _float(0), _complex(0,0),
    _identifier(), _function_id(FUNCTION_EXP), _arguments()
{
  // Pass..
}

AstNode::AstNode(double value)
  : _type(TYPE_FLOAT), _integer(0), _float(value), _complex(0,0),
    _identifier(), _function_id(FUNCTION_EXP), _arguments()
{
  // Pass..
}

AstNode::AstNode(const std::complex<double> &value)
  : _type(TYPE_COMPLEX), _integer(0), _float(0), _complex(value),
    _identifier(), _function_id(FUNCTION_EXP), _arguments()
{
  // Pass..
}

AstNode::AstNode(const std::string &name)
  : _type(TYPE_INTEGER), _integer(0), _float(0), _complex(0,0),
    _identifier(name), _function_id(FUNCTION_EXP), _arguments()
{
  // Pass..
}

AstNode::AstNode(Type type, AstNode *lhs, AstNode *rhs)
  : _type(type), _integer(0), _float(0), _complex(0,0),
    _identifier(), _function_id(FUNCTION_EXP), _arguments(2)
{
  _arguments[0] = lhs; _arguments[1] = rhs;
}

AstNode::AstNode(Function function, AstNode *arg1)
  : _type(TYPE_FUNCTION), _integer(0), _float(0), _complex(0,0),
    _identifier(), _function_id(function), _arguments(1)
{
  _arguments[0] = arg1;
}

AstNode::~AstNode() {
  for (size_t i=0; i<_arguments.size(); i++) {
    delete _arguments[i];
  }
}
