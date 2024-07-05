#ifndef EVSPACE_EVAL_BASIS_SORT_H_
#define EVSPACE_EVAL_BASIS_SORT_H_

#include "eval/basis/basis.h"

namespace EVSPACE {
namespace EVAL {
namespace BASIS {
namespace SORT {

DECLARE_BASIS(SortBasis);
#define DECLARE_CONCRETE_SORT_BASIS(CONCRETE, R, ...) \
  DECLARE_CONCRETE_BASIS(SortBasis, CONCRETE, R, __VA_ARGS__)

} // SORT
} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_SORT_H_ */
