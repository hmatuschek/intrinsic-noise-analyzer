/**
 * @defgroup ast The Abstract Syntax Tree
 *
 * The Ast namespace collects all classes representing the abstract syntax tree (AST).
 *
 * \section features Supported SBML l2v4 features
 * Beside the commen (base) features of SBML l2v4, the following "extended" features are supported:
 *
 * - User defined functions
 * - Assignment rules
 * - RateLaws
 * - Local parameters for KineticLaw
 * .
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

#include "assembler.hh"
#include "evaluate.hh"

#include "trafo/trafo.hh"

#endif // __FLUC_COMPILER_AST_HH__
