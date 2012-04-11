#ifndef __FLUC_AST_TRAFO_MODELWALKER_HH__
#define __FLUC_AST_TRAFO_MODELWALKER_HH__


#include "walker.hh"
#include "ast/model.hh"
#include "ast/species.hh"
#include "ast/parameter.hh"
#include "ast/compartment.hh"
#include "ast/functiondefinition.hh"



namespace Fluc {
namespace Ast {
namespace Trafo {


/**
 * This class extends the @c Ast::Trafo::Walker to provide specialized handler for
 * parameter and species definitions.
 *
 * @ingroup trafo
 */
class ModelWalker : public Walker
{
public:
  /**
   * Constructor.
   *
   * Does actually nothing.
   */
  ModelWalker();

  /**
   * Processes all @c Ast::Node instances of the model.
   */
  virtual void handleModel(Ast::Model *node);

  /**
   * By default, implements a dispatcher to call one of the following specialized handlers:
   * @c handleVariableDefinition, @c handleSpeciesDefinition, @c handleParameterDefinition or
   * @c handleFunctionArgumentDefinition.
   */
  virtual void handleVariableDefinition(Ast::VariableDefinition *node);

  /**
   * Handles a species definition.
   *
   * Simply calls @c Ast::Trafo::Walker::processVariableDefinition.
   */
  virtual void handleSpeciesDefinition(Ast::Species *node);

  /**
   * Handles a parameter definition.
   *
   * Simply calls @c Ast::Trafo::Walker::processVariableDefinition.
   */
  virtual void handleParameterDefinition(Ast::Parameter *node);

  /**
   * Handles a compartment definition.
   *
   * Simply calls @c Ast::Trafo::Walker::processVariableDefinition.
   */
  virtual void handleCompartmentDefinition(Ast::Compartment *node);

  /**
   * Handles a function argument definition.
   */
  virtual void handleFunctionArgumentDefinition(Ast::FunctionArgument *node);
};


}
}
}

#endif // __FLUC_AST_TRAFO_MODELWALKER_HH__
