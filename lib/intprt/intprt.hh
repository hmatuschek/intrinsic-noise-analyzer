/**
 * @defgroup intprt Simple Byte-Code Interpreter
 *
 * This module implements a simple byte-code interpreter for GiNaC expressions, that allows
 * to evaluate "pre-compiled" expressions much faster, than using the built-in evaluation
 * mechanism of GiNaC.
 *
 * The evaluation scheme of the interpreter is a stack-machine \cite aho2007, but some operands
 * may me provided directly to the instruction as immediate values. This reduces the size of code
 * and reduces the number of stack-accesses.
 *
 * @todo Implement a common evaluation interface similar to this one. The evaluation should be
 *       separated into 2 steps: Compilation from GiNaC expressions and symbol resolution and
 *       finally evaluation. There should be at least two evaluation schemes: Direct evaluation of
 *       GiNaC expression using a symbol->value table and the interpretation of byte-code.
 */


#include "instruction.hh"
#include "codestatistics.hh"
#include "pass.hh"
#include "compiler.hh"
#include "interpreter.hh"
