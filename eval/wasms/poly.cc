#include <emscripten.h>
#include <stddef.h>
#include <malloc.h>
#include "../evals/poly.h"


// Prevent mangling from C++ so that can be called from js
extern "C" {
  double PolyFunction(double *sequences, size_t size, double x);
  double PthreadTest();
}


EMSCRIPTEN_KEEPALIVE
double PolyFunction(double *sequences, size_t size, double x) {
  return EVAL::EVALS::EvalPolyFunc(sequences, size, x);
}

EMSCRIPTEN_KEEPALIVE
double PthreadTest() {
  return EVAL::EVALS::PthreadTest();
}
