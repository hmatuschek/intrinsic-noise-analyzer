#ifndef __INA_AST_VISITOR_HH__
#define __INA_AST_VISITOR_HH__

namespace iNA {
namespace Ast {

/** Base class of all visitors.
 * @ingroup ast */
class Visitor
{
public:
  /** Just there to ensure vtable generation custom visitor and a common base-class. */
  virtual ~Visitor();
};


/** Base class of all operators.
 * @ingroup ast */
class Operator
{
public:
  /** Just there to ensure vtable generation custom operator and a common base-class. */
  virtual ~Operator();
};

}
}

#endif // VISITOR_HH
