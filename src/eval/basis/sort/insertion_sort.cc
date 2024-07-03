#include "insertion_sort.h"

#include <algorithm>

namespace EVSPACE {
namespace EVAL {
namespace BASIS {
namespace SORT {


void InsertionSort::operator()(double *seq, size_t size) {
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


} // SORT
} // BASIS
} // EVAL
} // EVSPACE
