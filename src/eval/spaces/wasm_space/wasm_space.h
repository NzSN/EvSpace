#ifndef EVSPACE_EVAL_SPACE_H_
#define EVSPACE_EVAL_SPACE_H_

#include <emscripten.h>
#include "eval/basis/basis_declarations.h"

#define GEN_SIGNATURE(BASIS, R, ...)            \
  EMSCRIPTEN_KEEPALIVE R BASIS##w (__VA_ARGS__)
#define PRINT_PARA(...) __VA_ARGS__
#define AS_WASM_BASIS(B, SIGNATURE, PARA_NAME, TYPES) \
  extern "C" { SIGNATURE(GEN_SIGNATURE); }                \
  SIGNATURE(GEN_SIGNATURE) {                              \
    return EVSPACE::BASIS::DECL::B(PARA_NAME(PRINT_PARA));                  \
  }

#define SPAN_WASM_SPACE_FROM_BASIS(BASIS, R, ...) \
  AS_WASM_BASIS(BASIS, R, __VA_ARGS__)

#endif /* EVSPACE_EVAL_SPACE_H_ */
