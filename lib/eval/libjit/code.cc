#include "code.hh"

using namespace Fluc::Evaluate::libjit;



Code::Code()
{
  // Create JIT context create:
  this->context = jit_context_create();

  // Create function signature:
  jit_type_t params[2];
  jit_type_t signature;
  params[0] = jit_type_create_pointer(jit_type_void_ptr, 1);
  params[1] = jit_type_create_pointer(jit_type_void_ptr, 1);
  signature = jit_type_create_signature(jit_abi_cdecl, jit_type_void, params, 2, 1);

  // Allocate function:
  this->function = jit_function_create(this->context, signature);
}


Code::~Code()
{
  // Free JIT context:
  jit_context_destroy(this->context);
}


jit_function_t
Code::getFunction()
{
  return this->function;
}
