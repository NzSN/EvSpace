#include <pthread.h>
#include "poly.h"


namespace EVAL {
namespace EVALS {

double EvalPolyFunc(double *sequences, size_t size, double x) {
  double result = 0;

  for (int i = 0; i < size; ++i) {
    result += sequences[i] * pow(x,2);
  }

  return result;
}

void* PlusOne(void *num) {
  double* num_ = (double*)(num);
  *num_ += 1;

  return NULL;
}

double PthreadTest() {
  int count = 0;

  pthread_t t0;
  if (pthread_create(&t0, NULL, PlusOne, &count) != 0) {
    return 4;
  }

  pthread_t t1;
  if (pthread_create(&t1, NULL, PlusOne, &count) != 0) {
    return 5;
  }

  pthread_join(t0, NULL);
  pthread_join(t1, NULL);

  return count;
}


} // EVALS
} // EVAL
