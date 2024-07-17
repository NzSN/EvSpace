#ifndef EVSPACE_EVAL_SPACES_NODE_NATIVE_SPACE_CODEGEN_H_
#define EVSPACE_EVAL_SPACES_NODE_NATIVE_SPACE_CODEGEN_H_

#include <cstdint>
#include <string>
#include <tuple>
#include <type_traits>
#include <optional>
#include <type_traits>

#define NAPI_DISABLE_CPP_EXCEPTIONS
#include <napi.h>

namespace EVSPACE {
namespace EVAL {
namespace SPACE {
namespace NODE_NATIVE_SPACE {
namespace CODEGEN {


template<typename T, typename = void>
struct NativeReduction;
template<>
struct NativeReduction<double> {
  using type = Napi::Number;
  static bool check(const Napi::Value& value) {
    return value.IsNumber();
  }
};
template<>
struct NativeReduction<size_t> {
  using type = Napi::Number;
  static bool check(const Napi::Value& value) {
    return value.IsNumber();
  }
};
template<typename T>
struct NativeReduction<T*, std::enable_if_t<
                             std::is_same_v<T, double>   ||
                             std::is_same_v<T, float>    ||
                             std::is_same_v<T, uint32_t> ||
                             std::is_same_v<T, int32_t>>> {
  using type = Napi::TypedArray;
  static bool check(const Napi::Value& value) {
    if (value.IsTypedArray()) {
      Napi::TypedArray typedArray = value.As<Napi::TypedArray>();
      return postCheck(typedArray);
    } else {
      return false;
    }
  }
  static bool postCheck(Napi::TypedArray& tArray) {
    if (std::is_same_v<T, double>) {
      return tArray.TypedArrayType() == napi_float64_array;
    } else if (std::is_same_v<T, float>)  {
      return tArray.TypedArrayType() == napi_float32_array;
    } else if (std::is_same_v<T, uint32_t>) {
      return tArray.TypedArrayType() == napi_uint32_array;
    } else if (std::is_same_v<T, int32_t>) {
      return tArray.TypedArrayType() == napi_int32_array;
    } else {
      return false;
    }
  }
};
template<>
struct NativeReduction<std::string> {
  using type = Napi::String;
  static bool check(const Napi::Value& value) {
    return value.IsString();
  }
};


template<typename... Args>
struct NativeType {
  using type = std::tuple<typename NativeReduction<Args>::type...>;
};
template<size_t num, typename... Args>
std::optional<typename NativeType<Args...>::type>
AsNatives(const Napi::CallbackInfo& info) {
  if (info.Length() != num) {
    return std::nullopt;
  }

  size_t pos = 0;
  (NativeReduction<Args>::check(info[pos++]), ...);

  pos = 0;
  return std::optional{
    std::make_tuple(info[pos++].As<NativeReduction<Args>>()...)};
}

#define NUM_OF_PARAM(_1, _2, _3, _4, _5, _NAME, ...) _NAME
#define NATIVE_TUPLE_RECEIVER __NAT_TUPLE
#define AS_NATIVES(R, ...)                                              \
  auto NATIVE_TUPLE_RECEIVER = CODEGEN::AsNatives<                               \
    NUM_OF_PARAM(__VA_ARGS__, 5, 4, 3, 2, 1, 0),                        \
    __VA_ARGS__>(info);                                                 \
  if (!NATIVE_TUPLE_RECEIVER.has_value()) {                                          \
    Napi::Error::New(info.Env(), "Wrong argument");                     \
    return info.Env().Undefined();                                      \
  }

#define EVAL_NATIVE_BASIS_IMPL_0(DECLARE)       \
  return DECLARE();
#define EVAL_NATIVE_BASIS_IMPL_1(DECLARE)       \
  return DECLARE(std::get<0>(NATIVE_TUPLE_RECEIVER.value()));
#define EVAL_NATIVE_BASIS_IMPL_2(DECLARE)       \
  return DECLARE(std::get<0>(NATIVE_TUPLE_RECEIVER.value()),      \
                 std::get<1>(NATIVE_TUPLE_RECEIVER.value()));
#define EVAL_NATIVE_BASIS_IMPL_3(DECLARE)       \
  return DECLARE(std::get<0>(NATIVE_TUPLE_RECEIVER.value()),      \
                 std::get<1>(NATIVE_TUPLE_RECEIVER.value()),      \
                 std::get<2>(NATIVE_TUPLE_RECEIVER.value()));
#define EVAL_NATIVE_BASIS_IMPL_4(DECLARE)       \
  return DECLARE(std::get<0>(NATIVE_TUPLE_RECEIVER.value()),      \
                 std::get<1>(NATIVE_TUPLE_RECEIVER.value()),      \
                 std::get<2>(NATIVE_TUPLE_RECEIVER.value()),      \
                 std::get<3>(NATIVE_TUPLE_RECEIVER.value()));
#define EVAL_NATIVE_BASIS_IMPL_5(DECLARE)       \
   return DECLARE(std::get<0>(NATIVE_TUPLE_RECEIVER.value()),     \
                  std::get<1>(NATIVE_TUPLE_RECEIVER.value()),     \
                  std::Get<2>(NATIVE_TUPLE_RECEIVER.value()),     \
                  std::Get<3>(NATIVE_TUPLE_RECEIVER.value()),     \
                  std::Get<4>(NATIVE_TUPLE_RECEIVER.value()));
#define SELECT_NATIVE_BASIS(_R, _1, _2, _3, _4, _5, _NAME, ...) \
  EVAL_NATIVE_BASIS_IMPL_##_NAME
#define EVAL_NATIVE_BASIS(...)                        \
  SELECT_NATIVE_BASIS(__VA_ARGS__, 5, 4, 3, 2, 1, 0)
#define PARAM_TRANS_IMPL_0(DECLARE, SIGNATURE, PARA_NAME, PARA_TYPES) \
  /* Base Case: Do nothing */
#define PARAM_TRANS_IMPL_N(DECLARE, SIGNATURE, PARA_NAME, PARA_TYPES) \
  PARA_TYPES(AS_NATIVES)
#define SELECT_BY_NUM_OF_PARAM(_1, _2, _3, _4, _5, _NAME, ...)  \
  PARAM_TRANS_IMPL_##_NAME
#define PARAM_TRANS_IMPL(...)                           \
  SELECT_BY_NUM_OF_PARAM(__VA_ARGS__, N, N, N, N, N, 0)

#define NODE_NATIVE_WRAPPER_PARAM_TRANS(        \
  DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE)     \
  PARA_NAME(PARAM_TRANS_IMPL)(                  \
    DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE)  \

#define NODE_NATIVE_WRAPPER_EVAL( \
  DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE)           \
  PARA_TYPE(EVAL_NATIVE_BASIS)(DECLARE)

#define SPAWN_NODE_NATIVE_WRAPPER_SIGNATURE(    \
  DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE)     \
  Napi::Value DECLARE(const Napi::CallbackInfo& info) { \
    NODE_NATIVE_WRAPPER_PARAM_TRANS(            \
      DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE); \
    NODE_NATIVE_WRAPPER_EVAL(                   \
      DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE); \
  }


} // CODGEN
} // NODE_NATIVE_SPACE
} // SPACE
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_SPACES_NODE_NATIVE_SPACE_CODEGEN_H_ */
