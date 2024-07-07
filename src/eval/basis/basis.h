#ifndef EVSPACE_EVAL_BASIS_BASIS_H_
#define EVSPACE_EVAL_BASIS_BASIS_H_

#include "proxy/proxy.h"

namespace EVSPACE {
namespace EVAL {
namespace BASIS {

PRO_DEF_MEMBER_DISPATCH(__NAME, ...)

// Work as type wrapper to define Basis in
// type system.
struct Basis {};

#define DECLARE_BASIS_OPERATOR(R, ...) \
  static R operator()(__VA_ARGS__)
#define DEFINE_BASIS_OPERATOR(B, R, ...) \
  R B::operator()(__VA_ARGS__)
#define DECLARE_BASIS(B) struct B: public Basis {}
#define DECLARE_CONCRETE_BASIS(ABSTRACT, CONCRETE, R, ...)            \
  struct CONCRETE: public ABSTRACT {                                  \
    CONCRETE() {                                                      \
      static_assert(std::is_invocable_r_v<R, CONCRETE, __VA_ARGS__>); \
    }                                                                 \
    DECLARE_BASIS_OPERATOR(R, __VA_ARGS__);                           \
  };

#define DEFINE_CONCRETE_BASIS(CONCRETE, R, ...) \
  DEFINE_BASIS_OPERATOR(CONCRETE, R, __VA_ARGS__)

} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_BASIS_H_ */
