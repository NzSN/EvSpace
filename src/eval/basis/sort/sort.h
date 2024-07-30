#ifndef EVSPACE_EVAL_BASIS_SORT_H_
#define EVSPACE_EVAL_BASIS_SORT_H_

#include "eval/basis/basis.h"

namespace EVSPACE {
namespace EVAL {
namespace BASIS {
namespace SORT {

template<typename T>
struct SortBasis : Basis {
  virtual void operator()(T*, size_t) {}
};

} // SORT
} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_SORT_H_ */
