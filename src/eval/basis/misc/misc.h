#ifndef EVSPACE_EVAL_BASIS_MISC_MISC_H_
#define EVSPACE_EVAL_BASIS_MISC_MISC_H_

#include "eval/basis/basis.h"
#include "eval/async/pipe.h"

namespace EVSPACE {
namespace EVAL {
namespace BASIS {
namespace MISC {

template<typename T>
struct AsyncBasis : Basis {
  virtual ASYNCHRONOUS(T) operator()() {
    return {};
  };
};

} // MISC
} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_MISC_MISC_H_ */
