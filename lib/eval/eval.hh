#ifndef __FLUC_EVAL_HH__
#define __FLUC_EVAL_HH__

#include "../config.hh"


/**
 * @defgroup eval Expression Evaluation
 *
 * This group contains a set of classes implementing different evaluation mechanisms from very slow
 * direct evaluation of GiNaC expressions to JIT compilers.
 *
 * All evaluation methods share the same interface, so they can be used as a template argument to
 * make the engine selectable at compile time.
 */

#include "direct/engine.hh"
#include "direct/code.hh"
#include "direct/compiler.hh"
#include "direct/interpreter.hh"

#include "bci/engine.hh"
#include "bci/code.hh"
#include "bci/compiler.hh"
#include "bci/interpreter.hh"

#include "bcimp/engine.hh"
#include "bcimp/code.hh"
#include "bcimp/compiler.hh"
#include "bcimp/interpreter.hh"

#if WITH_EXECUTION_ENGINE_LLVM
#include "jit/engine.hh"
#include "jit/code.hh"
#include "jit/compiler.hh"
#include "jit/interpreter.hh"
#endif




#endif  
