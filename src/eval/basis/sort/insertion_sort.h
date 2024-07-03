#ifndef EVSPACE_EVAL_BASIS_SORT_INSERTION_SORT_H_
#define EVSPACE_EVAL_BASIS_SORT_INSERTION_SORT_H_

#include <stddef.h>
#include "sort.h"

namespace EVSPACE {
namespace EVAL {
namespace BASIS {
namespace SORT {

struct InsertionSort: public SortBasis {
  static void operator()(double *seq, size_t size);
};

} // SORT
} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_SORT_INSERTION_SORT_H_ */
