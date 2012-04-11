#ifndef __FLUC_COMPILER_ASTDEFINITION_HH__
#define __FLUC_COMPILER_ASTDEFINITION_HH__

#include <string>
#include <ginac/symbol.h>

#include "node.hh"


namespace Fluc {
namespace Ast {


/**
 * Base class for all definitions, like function definitions etc.
 *
 * @ingroup ast
 */
class Definition : public Node
{
protected:
  /**
   * The identifier of the definiton.
   */
  std::string identifier;

  /**
   * Optional display name.
   */
  std::string name;


protected:
  /**
   * Protected constructor, avoids direct instantiation of the class.
   */
  Definition(const std::string &id, Node::NodeType node_type);

  /**
   * Constructor with optional display name.
   */
  Definition(const std::string &id, const std::string &name, Node::NodeType node_type);


public:
  /**
   * Destructor, does nothin (yet).
   */
  virtual ~Definition();

  /**
   * Returns the identifier/name of the definition.
   */
  const std::string &getIdentifier() const;

  /**
   * Returns true if there is a display-name assigned to the definition.
   */
  bool hasName() const;

  /**
   * Returns the display name of the definition (if there is one).
   */
  const std::string &getName() const;

  /**
   * Resets the display-name of the definition.
   */
  void setName(const std::string &name);
};


}
}

#endif
