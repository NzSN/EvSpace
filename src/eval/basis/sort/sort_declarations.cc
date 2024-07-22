#include "sort_declarations.h"
#include "eval/basis/sort/insertion_sort.h"

namespace SORT = EVSPACE::EVAL::BASIS::SORT;

void insertionSort(double *seq, size_t size) {
  SORT::InsertionSort<double> sort;
  return sort(seq, size);
}

void trivialSort(double* seq, size_t size, double* seq_2, size_t size_2) {
  return;
}
