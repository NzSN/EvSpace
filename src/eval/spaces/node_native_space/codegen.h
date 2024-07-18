#ifndef EVSPACE_EVAL_SPACES_NODE_NATIVE_SPACE_CODEGEN_H_
#define EVSPACE_EVAL_SPACES_NODE_NATIVE_SPACE_CODEGEN_H_

#include <cstdint>
#include <string>
#include <tuple>
#include <optional>
#include <type_traits>
#include <tuple>

#define NAPI_DISABLE_CPP_EXCEPTIONS
#include <napi.h>

namespace EVSPACE {
namespace EVAL {
namespace SPACE {
namespace NODE_NATIVE_SPACE {
namespace CODEGEN {

template<typename T>
struct type_identity { using type = T; };

template<typename T>
struct NativeReduction;

#define REDUCTION_TYPED_ARRAY_LIST(V)                     \
  /* From      TO                  Runtime check value */ \
  V(double*  , Napi::Float64Array, napi_float64_array)    \
  V(float*   , Napi::Float32Array, napi_float32_array)    \
  V(uint32_t*, Napi::Uint32Array , napi_uint32_array)     \
  V(int32_t* , Napi::Int32Array  , napi_int32_array)

#define REDUCTION_TO_NATIVE_PRIMITIVE_TYPE_LIST(V) \
  /*From    To            Runtime check */         \
  V(double, Napi::Number, IsNumber)                \
  V(size_t, Napi::Number, IsNumber)                \
  V(std::string, Napi::String, IsString)

#define TYPED_ARRAY_SIZE_TYPE_MAPPING_LIST(V)  \
  /* FROM TO */                                 \
  V(Napi::Float64Array, size_t)                 \
  V(Napi::Float32Array, size_t)                 \
  V(Napi::Uint32Array, size_t)                  \
  V(Napi::Int32Array, size_t)


#define DECLARE_REDUCTION(C_TYPE, N_TYPE, SUBTYPE)       \
  template<>                                             \
  struct NativeReduction<C_TYPE> {                       \
    using type = N_TYPE;                                 \
    static bool check(const Napi::Value& value) {        \
      return value.IsTypedArray() &&                     \
        value.As<Napi::TypedArray>().TypedArrayType() == \
        SUBTYPE;                                         \
    }                                                    \
  };

REDUCTION_TYPED_ARRAY_LIST(DECLARE_REDUCTION);
#undef DECLARE_REDUCTION

#define DECLARE_PRIMITIVE_REDUCTION_TO_NATIVE(C_TYPE, N_TYPE, CHECK) \
  template<>                                                         \
  struct NativeReduction<C_TYPE> {                                   \
    using type = N_TYPE;                                             \
    static bool check(const Napi::Value& value) {                    \
      return value.CHECK();                                          \
    }                                                                \
  };
REDUCTION_TO_NATIVE_PRIMITIVE_TYPE_LIST(DECLARE_PRIMITIVE_REDUCTION_TO_NATIVE)
#undef DECLARE_PRIMITIVE_REDUCTION_TO_NATIVE

template<typename T>
struct SizeTypeReduction {
  using type = void;
};
#define DECLARE_REDUCTION_FROM_NATIVE_TO_CTYPE(NT, CT) \
  template<>                                           \
  struct SizeTypeReduction<NT> {                          \
    using type = CT;                                   \
  };

TYPED_ARRAY_SIZE_TYPE_MAPPING_LIST(DECLARE_REDUCTION_FROM_NATIVE_TO_CTYPE);

template<typename... Ts>
struct Count {
  constexpr static size_t count = 0;
};
template<typename T, typename... Ts>
struct Count<T, Ts...> {
  constexpr static size_t count = 1 + Count<Ts...>::count;
};


template<typename... Ts>
struct Tlist;
template<typename T, typename... Ts>
struct Tlist<T, Tlist<Ts...>> {
  using type = std::tuple<T, Ts...>;
};
template<typename T, typename... Ts>
struct Tlist<Tlist<Ts...>, T> {
  using type = std::tuple<Ts..., T>;
};

template<size_t idx, typename... Ts>
struct TAtPos;
template<typename T>
struct TAtPos<0, T> {
  using type = T;
};
template<size_t idx, typename T, typename... Ts>
struct TAtPos<idx, T, Ts...> {
  static_assert(idx < Count<T, Ts...>::count,
    "Access to Tlist is out of range");
  using type = typename TAtPos<idx-1, Ts...>::type;
};

template<size_t idx, typename... Ts>
struct GetTlist;
template<size_t idx, typename... Ts>
struct GetTlist<idx, Tlist<Ts...>> {
  using type = typename TAtPos<idx, Ts...>::type;
};


template<template<typename,typename> class R, typename O, typename... Ts>
struct Reduce : type_identity<O> {};
template<template<typename,typename> class R, typename O, typename... Ts>
using Reduce_t = typename Reduce<R, O, Ts...>::type;
template<template<typename,typename> class R, typename O, typename T, typename... Ts>
struct Reduce<R, O, T, Ts...> {
  using type = Reduce_t<R, R<O, typename NativeReduction<T>::type>, Ts...>;
};
template<template<typename,typename> class R, typename O, typename T, typename U, typename... Ts>
struct Reduce<R, O, T*, U, Ts...> {
  // Declaration Layer Interface must declare parameter to indicate
  // valid range of a pointer at the pos next to where pointer parameter
  // is declared. Other wise treat as not-well form.
  //
  // Typically, the type U to indicate valid ranges should be size_t
  static_assert(std::is_same_v<U, size_t>,
                "Parameter to indicate valid range in Memory "
                "of pointer must at pos next to the pointer");

  // U is ignored due to such message are passed by Napi::TypeArray<T>.
  using type = Reduce_t<R, R<O, typename NativeReduction<T*>::type>, Ts...>;
};


template<typename... Ts>
struct CollectTypes { using type = std::tuple<Ts...>; };
template<typename... Ts, typename T>
struct CollectTypes<CollectTypes<Ts...>, T> {
  using type = std::tuple<Ts..., T>;
};

template<typename... Ts>
struct InfoProc;
template<typename... Ts>
struct InfoProc<std::tuple<Ts...>> {
  static std::optional<std::tuple<Ts...>>
  extract(const Napi::CallbackInfo& info) {
    size_t pos = 0;
    return std::make_tuple(info[pos++].As<Ts>()...);
  }
};

template<typename... Ts>
using NodeTypes = typename Reduce_t<CollectTypes, CollectTypes<>, Ts...>::type;
template<typename... Ts>
std::optional<NodeTypes<Ts...>>
AsNatives(const Napi::CallbackInfo& info) {
  using inferNativeType = typename Reduce_t<
    CollectTypes, CollectTypes<>, Ts...>::type;

  static_assert(std::is_same_v<inferNativeType,
                std::tuple<Napi::TypedArrayOf<double>>>);

  if (info.Length() != std::tuple_size_v<inferNativeType>) {
    return std::nullopt;
  }

  // TODO: RunTime typecheck implement

  return InfoProc<inferNativeType>::extract(info);
}

template<size_t I>
struct ArgIDX {};

template<typename T, typename U>
struct DeclareArg;
template<typename T, size_t I>
struct DeclareArg<T, ArgIDX<I>> {};

template<size_t pos, typename... Ts>
struct Declaring;
template<size_t pos, typename... Ts, typename... NTs>
struct Declaring<pos, Tlist<Ts...>, Tlist<NTs...>> {
  using type = typename Declaring<pos, std::tuple<>, Tlist<NTs...>, Ts...>::type;
};
template<size_t pos, typename... Os, typename... NTs>
struct Declaring<pos, std::tuple<Os...>, Tlist<NTs...>> {
  using type = std::tuple<Os...>;
};
template<size_t pos, typename... Os, typename T, typename U,
         typename... Ts, typename... NTs>
struct Declaring<pos, std::tuple<Os...>, Tlist<NTs...>, T*, U, Ts...> {
  // Must to gurantee that Napi-Type inference from T* match the type
  // we need to extract information out.
  using TheNodeType = typename GetTlist<pos,Tlist<NTs...>>::type;
  using InferType = typename NativeReduction<T*>::type;
  static_assert(std::is_same_v<TheNodeType, InferType>);
  static_assert(std::is_same_v<U, size_t>);

  using type = typename Declaring<
    pos+1,
    std::tuple<
      Os...,
      DeclareArg<T*,ArgIDX<pos>>,
      DeclareArg<U ,ArgIDX<pos>>>,
    Tlist<NTs...>, Ts...>
    ::type;
};
template<size_t pos, typename... Os, typename T, typename... Ts, typename... NTs>
struct Declaring<pos, std::tuple<Os...>, Tlist<NTs...>, T, Ts...> {
  static_assert(false);
  // Must to gurantee that Napi-Type inference from T* match the type
  // we need to extract information out.
  using TheNodeType = typename GetTlist<pos,Tlist<NTs...>>::type;
  using InferType = typename NativeReduction<T>::type;
  static_assert(std::is_same_v<TheNodeType, InferType>);

  using type = typename Declaring<
    pos + 1,
    std::tuple<
      Os...,
      DeclareArg<T,ArgIDX<pos>>>,
    Tlist<NTs...>, Ts...>
    ::type;
};

template<typename... Ts>
struct DeclareArg_Trait {
  static_assert(false, "Faled to match a DeclareArg<T, ArgIDX<I>>");
};
template<typename T, size_t I>
struct DeclareArg_Trait<DeclareArg<T, ArgIDX<I>>> {
  using type = T;
  constexpr static size_t pos = I;
};

#define NATIVE_TRANSLATION_LIST(V)               \
  /* NativeType,         CType,   Method */      \
  V(Napi::Number       , double , DoubleValue) \
  V(Napi::Float64Array , double*, Data)        \
  V(Napi::Float64Array , size_t , ElementSize)

template<typename T, typename U>
struct NativeTranslator;
#define DECLARE_TRANSLATOR(NT, CT, M)           \
  template<>                                    \
  struct NativeTranslator<NT, CT> {             \
    static CT trans(NT& v) {                    \
      return v.M();                             \
    }                                           \
  };

NATIVE_TRANSLATION_LIST(DECLARE_TRANSLATOR)
#undef DECLARE_TRANSLATOR

template<typename... Ts>
struct Evaluator;
template<typename FN, typename... Args, typename... Ts>
struct Evaluator<FN, std::tuple<Args...>, std::tuple<Ts...>> {
  template<typename Arg>
  static auto argTrans(std::tuple<Ts...>& t) {
    using trait = DeclareArg_Trait<Arg>;
    using ArgType = typename trait::type;
    using SelectedNativeType = typename GetTlist<trait::pos, Tlist<Ts...>>::type;
    using NativeReductT = typename NativeReduction<ArgType>::type;
    using SizeType = typename SizeTypeReduction<SelectedNativeType>::type;

    static_assert(trait::pos < Count<Ts...>::count, "");
    static_assert(std::is_same_v<NativeReductT, SelectedNativeType> ||
                  /* otherwise, it should be a size_t  */
                  (std::is_same_v<ArgType, size_t> &&
                   std::is_same_v<SizeType, size_t>));

    return NativeTranslator<
      SelectedNativeType,
      typename trait::type>::trans(std::get<trait::pos>(t));
  }

  static auto eval(FN fn, std::tuple<Ts...>& t) {
    return fn(argTrans<Args>(t)...);
  }
};

template<typename FN, typename... Ts>
struct EvalFlat;
template<typename FN, typename... FN_ARGs, typename... Ts>
struct EvalFlat<FN, std::tuple<FN_ARGs...>, std::tuple<Ts...>> {
  static auto eval(FN fn, std::tuple<Ts...>& t) {
    using ArgInfos = typename Declaring<
      0, Tlist<FN_ARGs...>, Tlist<Ts...>>::type;

    return Evaluator<FN, ArgInfos, std::tuple<Ts...>>::eval(fn, t);
  }
};

template<typename FN, typename... Ts>
auto Eval(FN fn, NodeTypes<Ts...> t) {
  static_assert(std::is_invocable_v<FN, Ts...>);
  return EvalFlat<FN, std::tuple<Ts...>, NodeTypes<Ts...>>::eval(fn, t);
}

#define NUM_OF_PARAM(_1, _2, _3, _4, _5, _NAME, ...) _NAME
#define NATIVE_TUPLE_RECEIVER __NAT_TUPLE
#define AS_NATIVES(BASIS, R, ...)                                       \
  auto NATIVE_TUPLE_RECEIVER = CODEGEN::AsNatives<                      \
    __VA_ARGS__>(info);                                                 \
  if (!NATIVE_TUPLE_RECEIVER.has_value()) {                             \
    Napi::Error::New(info.Env(), "Wrong argument");                     \
    return info.Env().Undefined();                                      \
  }

#define PARAM_TRANS_IMPL_0(DECLARE, SIGNATURE, PARA_NAME, PARA_TYPES) \
  /* Base Case: Do nothing */
#define PARAM_TRANS_IMPL_N(DECLARE, SIGNATURE, PARA_NAME, PARA_TYPES) \
  PARA_TYPES(AS_NATIVES)
#define SELECT_BY_NUM_OF_PARAM(_1, _2, _3, _4, _5, _NAME, ...)  \
  PARAM_TRANS_IMPL_##_NAME
#define PARAM_TRANS_IMPL(...)                           \
  SELECT_BY_NUM_OF_PARAM(__VA_ARGS__, N, N, N, N, N, 0)

#define EVAL(BASIS, R, ...) \
  return CODEGEN::Eval<R(*)(__VA_ARGS__), __VA_ARGS__>( \
    BASIS, NATIVE_TUPLE_RECEIVER.value())

#define NODE_NATIVE_WRAPPER_PARAM_TRANS(        \
  DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE)     \
  PARA_NAME(PARAM_TRANS_IMPL)(                  \
    DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE)   \

#define NODE_NATIVE_WRAPPER_EVAL(DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE) \
  PARA_TYPE(EVAL);

#define SPAWN_NODE_NATIVE_WRAPPER_SIGNATURE(    \
  DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE)     \
  Napi::Value _##DECLARE(const Napi::CallbackInfo& info) {              \
    NODE_NATIVE_WRAPPER_PARAM_TRANS(DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE); \
    NODE_NATIVE_WRAPPER_EVAL(DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE); \
  }


} // CODGEN
} // NODE_NATIVE_SPACE
} // SPACE
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_SPACES_NODE_NATIVE_SPACE_CODEGEN_H_ */
