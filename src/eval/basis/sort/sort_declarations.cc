#include <thread>
#include <pthread.h>

#include "sort_declarations.h"
#include "eval/basis/sort/insertion_sort.h"

namespace SORT = EVSPACE::EVAL::BASIS::SORT;

void insertionSort(double *seq, size_t size) {
  SORT::InsertionSort<double> sort;

  std::thread t{sort, seq, size};
  t.join();
  return;
}

void trivialSort(double* seq, size_t size) {
  return;
}
