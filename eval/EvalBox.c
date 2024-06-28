#include <emscripten.h>
#include <math.h>
#include <malloc.h>


EMSCRIPTEN_KEEPALIVE
double PolyFunction(double *sequences, size_t size, double x) {
  double result = 0;

  for (int i = 0; i < size; ++i) {
    result += sequences[i] * pow(x,2);
  }

  return result;
}
