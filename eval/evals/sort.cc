#include "sort.h"
#include <algorithm>
#include <ctime>
#include <time.h>

namespace EVAL {
namespace EVALS {

void InsertionSort(double *seq, size_t size) {
  if (size < 2) return;

  for (int i = 1; i < size; ++i) {
    int key = seq[i];

    int j = i - 1;
    while (j >= 0 && seq[j] > key) {
      std::swap(seq[j], seq[j+1]);
      --j;
    }
    seq[j + 1] = key;
  }
}

void Sort(double *sequences, size_t size) {
  InsertionSort(sequences, size);
}

double SortWithDebug(double *sequences, size_t size) {
  clock_t begin = clock();

  InsertionSort(sequences, size);

  clock_t end = clock();

  double time_spent = ((double)(end - begin) / CLOCKS_PER_SEC) * 1000;
  return time_spent;
}


} // EVALS
} // EVAL

