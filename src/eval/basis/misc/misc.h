#ifndef EVSPACE_EVAL_BASIS_MISC_MISC_H_
#define EVSPACE_EVAL_BASIS_MISC_MISC_H_

#include "base/assert.h"
#include "eval/basis/basis.h"
#include "eval/async/async.h"
#include <type_traits>

namespace EVSPACE {
namespace EVAL {
namespace BASIS {
namespace MISC {

template<typename T>
struct AsyncBasis : Basis {
  virtual ASYNCHRONOUS(T) operator()() {
    ASSERT(false,
           "AsyncBasis::operator() should not be called");
  };
};

} // MISC
} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_MISC_MISC_H_ */
