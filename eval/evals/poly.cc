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

struct Data {
  pthread_mutex_t mutex;
  double num;
};

void* PlusTenTimes(void *num) {
  struct Data* data = (struct Data*)(num);

  for (int i = 0; i < 100000; ++i) {

    pthread_mutex_lock(&data->mutex);

    data->num++;

    pthread_mutex_unlock(&data->mutex);

  }

  return NULL;
}

double PthreadTest() {
  struct Data data = {
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .num = 0
  };

  pthread_t t0;
  if (pthread_create(&t0, NULL, PlusTenTimes, &data) != 0) {
    return 1;
  }

  pthread_t t1;
  if (pthread_create(&t1, NULL, PlusTenTimes, &data) != 0) {
    return 1;
  }

  pthread_join(t0, NULL);
  pthread_join(t1, NULL);

  return data.num;
}


} // EVALS
} // EVAL
