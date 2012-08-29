/**
 * @defgroup ast Model Representation
 * @ingroup models
 *
 * The @c Fluc::Ast namespace collects all classes representing the abstract syntax tree (AST).
 * This AST is similar to the model repesentation of SBML, but in constrast to SBML, this
 * representation is closer to the abstract model, while the SBML model is closer to the actual
 * SBML format. This representation ignores SBML specific meta-information that is not needed to
 * perform the analyses as SBO terms and RDF meta tags.
 *
 * The most imporatant class in this module is @c Fluc::Ast::Model, which represents the complete
 * system with all species, parameters, compartments, units and reactions. Starting from this class,
 * the various analyses are constructed, as described in @ref models.
 */

#ifndef __FLUC_COMPILER_AST_HH__
#define __FLUC_COMPILER_AST_HH__

#include "node.hh"
#include "module.hh"
#include "model.hh"
#include "constraint.hh"

#include "definition.hh"
#include "variabledefinition.hh"
#include "unitdefinition.hh"
#include "rule.hh"
#include "functiondefinition.hh"
#include "reaction.hh"

#include "species.hh"
#include "compartment.hh"
#include "parameter.hh"

#include "evaluate.hh"
#include "visitor.hh"

#include "trafo/trafo.hh"
#include "unitconverter.hh"

#endif // __FLUC_COMPILER_AST_HH__
