#ifndef __INA_TRAFO_TRAFO_HH__
#define __INA_TRAFO_TRAFO_HH__

/**
 * @defgroup trafo Model manipulations and transformations.
 * @ingroup models
 *
 * This group collects several assertions and transformations of @c Fluc::Ast::Model instances that
 * ease the implementation of analyses.
 *
 * The some of the model assertions and transformations rely on visitor and operator patterns
 * implemented in the model representation (@ref ast). These patterns allow for a convenient
 * traversal of the model specification. A @c Fluc::Ast::Visitor is a simple visitor pattern, that
 * allows traverse the @c Fluc::Ast::Model tree but does not allow any modifition of it. In
 * constrast to that, the @c Fluc::Ast::Operator allows additionally modification on the
 * @c Fluc::Ast::Model tree. As an example, the assertions @c Fluc::Trafo::NoRateRuleAssertion,
 * @c Fluc::Trafo::NoConstSpeciesAssertion, @c Fluc::Trafo::NoAssignmentRuleAssertion,
 * @c Fluc::Trafo::NoAlgebraicConstraintAssertion, @c Fluc::Trafo::NoReversibleReactionAssertion,
 * @c Fluc::Trafo::ConstParameterAssertion, @c Fluc::Trafo::ConstCompartmentAssertion,
 * @c Fluc::Trafo::ConstStoichiometryAssertion and @c Fluc::Trafo::NoExplicitTimeDependenceAssertion
 * utilize the @c Ast::Visitor pattern to make their assertions on the model.
 *
 * As an example of the @c Fluc::Ast::Operator pattern, the @c Fluc::Trafo::Substitution operator
 * allows to make proper subsitutions of any expression within the model.
 *
 * The default implementation of the visitor/operator pattern automatically traverses the complete
 * @c Fluc::Ast::Model tree. You can avoid this behavior or do some fine control over the traversal
 * by reimplementing the corresponding handler. For example, if you want to handle all global
 * parameters but no local paramters of any @c Fluc::Ast::KineticLaw simply override the
 * handler for the @c KineticLaw instances and do not call the @c traversal method.
 *
 * @code{.cpp}
 * class Handler :
 *   public Fluc::Ast::Visitor, public Fluc::Ast::Parameter::Visitor,
 *   public Fluc::Ast::KineticLaw::Visitor
 * {
 * public:
 *   void visit(const Fluc::Ast::Parameter *param) {
 *    // Do something with the paramter.
 *   }
 *
 *   void visit(const Fluc::Ast::KineticLaw *law) {
 *    // as long as you do not call
 *    // law->traverse(*this);
 *    // the local paramters arn't processed
 *    }
 * }
 * @endcode
 *
 * This also implies that whenever you override a handler for a node that has child nodes (ie.
 * the local paramters of a @c Fluc::Ast::KineticLaw or the rule of a
 * @c Fluc::Ast::VariableDefinition, you need to explicitly call the traverse method.
 *
 * @code{.cpp}
 * class Handler :
 *   public Fluc::Ast::Visitor, public Fluc::Ast::KineticLaw::Visitor
 * {
 * public:
 *   void visit(const Fluc::Ast::KineticLaw *law) {
 *    // Handle local paramters:
 *    law->traverse(*this);
 *    // then, do something with the kinetic law.
 *    // ...
 *    }
 * }
 * @endcode
 */

#include "assertions.hh"
#include "substitution.hh"
#include "constantfolder.hh"
#include "assignmentruleinliner.hh"
#include "conservationanalysis.hh"
#include "reversiblereactionconverter.hh"
#include "partialmodel.hh"
#include "referencecounter.hh"
#include "modeldiff.hh"
#include "makeparamglobal.hh"

#endif
