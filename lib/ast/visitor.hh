#ifndef __INA_AST_VISITOR_HH__
#define __INA_AST_VISITOR_HH__

namespace Fluc {
namespace Ast {

/** Base class of all visitors.
 * @ingroup ast */
class Visitor
{
public:
  /** Just there to ensure vtable generation for visitor and a common base-class. */
  virtual ~Visitor();
};


/** Base class of all operators.
 * @ingroup ast */
class Operator
{
public:
  /** Just there to ensure vtable generation for operator and a common base-class. */
  virtual ~Operator();
};

}
}

#endif // VISITOR_HH
