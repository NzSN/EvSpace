#ifndef EVSPACE_EVAL_BASIS_SORT_H_
#define EVSPACE_EVAL_BASIS_SORT_H_

#include "proxy/proxy.h"
#include "eval/basis/basis.h"
#include <ranges>
#include <type_traits>
#include <span>

namespace EVSPACE {
namespace EVAL {
namespace BASIS {
namespace SORT {

template<typename T>
concept OrderableAndSpanble = requires(T t , T u, size_t size) {
  /* All sort basis deal with
   * std::span<T> instead of T* */
  std::span{&t, size};
  /* Order operator */
  t < u;
  t <= u;
  t > u;
  t >= u;
  t == u;
};

template<OrderableAndSpanble T>
using SortBasisUnit = ProBasis<
  void(std::span<T>&),
  void(std::span<T>&&)>;

} // SORT
} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_SORT_H_ */
