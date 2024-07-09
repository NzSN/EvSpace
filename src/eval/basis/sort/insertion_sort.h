#ifndef EVSPACE_EVAL_BASIS_SORT_INSERTION_SORT_H_
#define EVSPACE_EVAL_BASIS_SORT_INSERTION_SORT_H_

#include <algorithm>
#include <concepts>
#include <stddef.h>
#include "eval/basis/basis.h"
#include "sort.h"


namespace EVSPACE {
namespace EVAL {
namespace BASIS {
namespace SORT {

template<OrderableAndSpanble T,
         typename S = std::span<T>>
struct InsertionSort {
  void operator()(S &seq) {
    auto size = seq.size();

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
