#include <emscripten.h>
#include "../evals/sort.h"

extern "C" {
  void Sort(double *seq, size_t size);
  double SortWithDebug(double *seq, size_t size);
}

EMSCRIPTEN_KEEPALIVE
void Sort(double *seq, size_t size) {
  EVAL::EVALS::Sort(seq, size);
}

EMSCRIPTEN_KEEPALIVE
double SortWithDebug(double *seq, size_t size) {
  return EVAL::EVALS::SortWithDebug(seq, size);
}
