#ifndef EVSPACE_EVAL_BASIS_BASIS_H_
#define EVSPACE_EVAL_BASIS_BASIS_H_

#include <type_traits>

namespace EVSPACE {
namespace EVAL {
namespace BASIS {


template<typename T, typename... Overloads>
struct BasisImpl;

template<typename T, typename R, typename... Args>
struct BasisImpl<T, R(Args...)> {
  BasisImpl() {
    static_assert(std::is_invocable_r<R, T, Args...>());
  }

 
  R operator()(Args... args) {
    return static_cast<T*>(this)->operator()(args...);
  }
};

// TODO: Support arbitary Overloads
template<typename T, typename Overloads>
struct Basis : BasisImpl<T, Overloads> {};


} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_BASIS_H_ */
