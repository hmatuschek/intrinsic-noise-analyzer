/**
 * @defgroup trafo Abstract Syntax Tree Transformations
 * @ingroup ast
 *
 * There are basically two kinds of maniplulation mechanisms that can be applied on the abstract
 * syntax tree (AST).
 *
 * The simplest mechanism are the 'direct' manipulations based on the
 * @c Fluc::Compiler::Trafo::Walker and the @c Fluc::Compiler::Trafo::Pass classes. These
 * classes traverse the complete AST and derived classes can perform their manipulations directly
 * on @c Fluc::Compiler::Ast::Node instances by overriding methods of the walker or pass classes.
 *
 * There is also an mechanism, specialized to maniplulate @c Fluc::Compiler::Ast::Expression
 * instances. This mechanism can be called 'rule-based', because it applies a set of
 * @c Fluc::Compiler::Trafo::TransformationRule instances recursively on expressions until no rule
 * can be applied on the expression anymore. The following sections describe these methods in more
 * detail.
 *
 * @section direct_trafo Direct AST Manipulations
 * There is a mechanism to manipulate the complete abstact syntax tree (AST) using the
 * @c Fluc::Compiler::Trafo::Walker and @c Fluc::Compiler::Trafo::Pass classes. These classes also
 * allow to modify expressions. Unfortunately these classes are unsuitable to perform manipulations
 * of expressions bases on patterns. These pattern-based expression manipulations are needed to
 * perform simple constant folding and other 'algebraic' manipulations.
 *
 * @section rule_trafo Pattern or Rule based Manipulations
 * To ease the implementation of pattern-based expression transformations the
 * @c Fluc::Compiler::Trafo::Transformation class was implemented. A transformation consists of a
 * set of @c Fluc::Compiler::Trafo::TransformationRule instances, that are applied recursively on
 * expressions.
 *
 * A @c Fluc::Compiler::Trafo::TransformationRule consists of a @c Fluc::Compiler::Trafo::Pattern
 * that determines if a rule can be applied on an expression and a
 * @c Fluc::Compiler::Trafo::Template that assembles the transformed expression. The template may
 * refer to sub-expressions, matched by the pattern to assemble the transformed expression.
 */

#ifndef __FLUC_COMPILER_TRAFO_TRAFO_HH__
#define __FLUC_COMPILER_TRAFO_TRAFO_HH__

#include "walker.hh"
#include "pass.hh"
#include "modelwalker.hh"

#include "constantfolder.hh"
#include "assignmentruleinliner.hh"
#include "substitutioncollector.hh"

#endif
