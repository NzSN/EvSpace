#ifndef EVSPACE_EVAL_SPACES_NODE_NATIVE_SPACE_CODEGEN_H_
#define EVSPACE_EVAL_SPACES_NODE_NATIVE_SPACE_CODEGEN_H_

#include <cstdint>
#include <string>
#include <type_traits>
#include <concepts>
#include <utility>
#include <optional>

#define NAPI_DISABLE_CPP_EXCEPTIONS
#include <napi.h>

namespace EVSPACE {
namespace EVAL {
namespace SPACE {
namespace NODE_NATIVE_SPACE {
namespace CODGEN {

// template<typename T>
// concept TypeReduction =
//   requires { T::type; } &&
//   requires (T::type &t) {
//   /* Runtime checking is required when there are potential
//    * differeent between instance of the T::type */
//   { T::runtimeChecking(t) } -> std::convertible_to<bool>;
// };

template<typename T>
struct NativeReduction;
template<>
struct NativeReduction<double> {
  using type = Napi::Number;
};

/* TypedArray */
template<typename T>
// requires(
//   std::is_same_v<T, double> ||
//   std::is_same_v<T, float> ||
//   std::is_same_v<T, uint32_t> ||
//   std::is_same_v<T, int32_t>)
struct NativeReduction<T*> {
  using type = Napi::TypedArray;
  static bool runtimeChecking(type& t) {
    if (std::is_same_v<T, double>) {
      return t.TypedArrayType() == napi_float64_array;
    } else if (std::is_same_v<T, float>)  {
      return t.TypedArrayType() == napi_float32_array;
    } else if (std::is_same_v<T, uint32_t>) {
      return t.TypedArrayType() == napi_uint32_array;
    } else if (std::is_same_v<T, int32_t>) {
      return t.TypedArrayType() == napi_int32_array;
    } else {
      return false;
    }
  }
};

/* String */
template<>
struct NativeReduction<std::string> {
  using type = Napi::String;
  bool runtimeChecking(type& t) {
    return true;
  }
};

template<typename... Args>
struct NativeTrans;

template<typename Arg, typename... Args>
struct NativeTrans<Arg, Args...> {
  using type = std::tuple<typename NativeReduction<Args>::type...>;
};

template<size_t num, typename... Args>
std::optional<typename NativeTrans<Args...>::type>
AsNatives(const Napi::CallbackInfo &info) {
  if (info.Length() != num) {
    return std::nullopt;
  }


}

#define PARAM_TRANS_IMPL_0(DECLARE, META, SIGNATURE, PARA_NAME, PARA_TYPES) \
  /* Base Case: Do nothing */
#define PARAM_TRANS_IMPL_1(DECLARE, META, SIGNATURE, PARA_NAME, PARA_TYPES)
#define PARAM_TRANS_IMPL_2(DECLARE, META, SIGNATURE, PARA_NAME, PARA_TYPES)
#define PARAM_TRANS_IMPL_3(DECLARE, META, SIGNATURE, PARA_NAME, PARA_TYPES)
#define PARAM_TRANS_IMPL_4(DECLARE, META, SIGNATURE, PARA_NAME, PARA_TYPES)
#define PARAM_TRANS_IMPL_5(DECLARE, META, SIGNATURE, PARA_NAME, PARA_TYPES)
#define SELECT_BY_NUM_OF_PARAM(_1, _2, _3, _4, _5, _NAME, ...)  \
  PARAM_TRANS_IMPL_##_NAME
#define PARAM_TRANS_IMPL(...)                           \
  SELECT_BY_NUM_OF_PARAM(__VA_ARGS__, 5, 4, 3, 2, 1, 0)

#define NODE_NATIVE_WRAPPER_PARAM_TRANS(              \
  DECLARE, META, SIGNATURE, PARA_NAME, PARA_TYPES)    \
                                                      \
  PARA_NAME(PARAM_TRANS_IMPL)(                        \
    DECLARE, META, SIGNATURE, PARA_NAME, PARA_TYPES)  \

#define NODE_NATIVE_WRAPPER_RETURN_TRANS(DECLARE)
#define SPAWN_NODE_NATIVE_WRAPPER_SIGNATURE(DECLARE)      \
  Napi::Value _DECLARE(const Napi::CallbackInfo& info) {  \
    NODE_NATIVE_WRAPPER_PARAM_TRANS(DECLARE);             \
    return NODE_NATIVE_WRAPPER_RETURN_TRANS(DECLARE);     \
  }


} // CODGEN
} // NODE_NATIVE_SPACE
} // SPACE
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_SPACES_NODE_NATIVE_SPACE_CODEGEN_H_ */
