#ifndef EVSPACE_EVAL_SPACES_NODE_NATIVE_SPACE_CODEGEN_H_
#define EVSPACE_EVAL_SPACES_NODE_NATIVE_SPACE_CODEGEN_H_

#include <cstdint>
#include <napi.h>
#include <type_traits>

namespace EVSPACE {
namespace EVAL {
namespace SPACE {
namespace NODE_NATIVE_SPACE {
namespace CODGEN {

template<typename>
struct NodeNumberTypeReduction {

};

template<typename T>
struct NodeTypesReduction;
template<>
struct NodeTypesReduction<double> {
  using type = Napi::Number;
};
template<typename T>
requires(
  std::is_same_v<T, double> ||
  std::is_same_v<T, uint32_t> ||
  std::is_same_v<T, int32_t>)
struct NodeTypesReduction<T*> {
  using type = Napi::TypedArray;
};

} // CODGEN
} // NODE_NATIVE_SPACE
} // SPACE
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_SPACES_NODE_NATIVE_SPACE_CODEGEN_H_ */
