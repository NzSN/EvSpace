#ifndef EVSPACE_EVAL_BASIS_SORT_INSERTION_SORT_H_
#define EVSPACE_EVAL_BASIS_SORT_INSERTION_SORT_H_

#include <algorithm>
#include <stddef.h>

#include "sort.h"


namespace EVSPACE {
namespace EVAL {
namespace BASIS {
namespace SORT {

template<typename T>
struct InsertionSort : SortBasis<T> {
  void operator()(T* seq, size_t size) final {
    if (size < 2) return;

    for (size_t i = 1; i < size; ++i) {
        int key = seq[i];

        int j = i - 1;
        while (j >= 0 && seq[j] > key) {
        std::swap(seq[j], seq[j+1]);
        --j;
        }
        seq[j + 1] = key;
    }
  }
};

} // SORT
} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_SORT_INSERTION_SORT_H_ */
