#ifndef EVSPACE_EVAL_BASIS_SORT_INSERTION_SORT_H_
#define EVSPACE_EVAL_BASIS_SORT_INSERTION_SORT_H_

#include <algorithm>
#include <stddef.h>
#include "eval/basis/basis.h"
#include "sort.h"

namespace EVSPACE {
namespace EVAL {
namespace BASIS {
namespace SORT {

DECLARE_CONCRETE_SORT_BASIS(
  InsertionSort,   /* Concrete */
  void,            /* Return type of operator */
  double*, size_t  /* Parameter of operator */);

} // SORT
} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_SORT_INSERTION_SORT_H_ */
