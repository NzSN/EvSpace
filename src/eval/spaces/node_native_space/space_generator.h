#ifndef EVSPACE_EVAL_SPACES_NODE_NATIVE_SPACE_CODEGEN_H_
#define EVSPACE_EVAL_SPACES_NODE_NATIVE_SPACE_CODEGEN_H_

#include <cstdint>
#include <string>
#include <tuple>
#include <optional>
#include <type_traits>
#include <tuple>

#include "eval/async/pipe.h"
#include "base/utility.h"

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
struct MAPPING_TO_NATIVE;

#define TYPED_ARRAY_MAPPING_LIST(V)                       \
  /* From      TO                  Runtime check value */ \
  V(double*  , Napi::Float64Array, napi_float64_array)    \
  V(float*   , Napi::Float32Array, napi_float32_array)    \
  V(uint32_t*, Napi::Uint32Array , napi_uint32_array)     \
  V(int32_t* , Napi::Int32Array  , napi_int32_array)

#define PRIMITIVE_TYPE_MAPPING_LIST(V)          \
  /*From    To            Runtime check */      \
  V(double      , Napi::Number, IsNumber)       \
  V(size_t      , Napi::Number, IsNumber)       \
  V(std::string , Napi::String, IsString)       \
  V(float       , Napi::Number, IsNumber)       \
  V(uint32_t    , Napi::Number, IsNumber)       \
  V(int32_t     , Napi::Number, IsNumber)

#define TYPE_MAPPING_LIST(V)                    \
  TYPED_ARRAY_MAPPING_LIST(V)                   \
  PRIMITIVE_TYPE_MAPPING_LIST(V)

#define TYPED_ARRAY_SIZE_TYPE_MAPPING_LIST(V)  \
  /* FROM TO */                                 \
  V(Napi::Float64Array, size_t)                 \
  V(Napi::Float32Array, size_t)                 \
  V(Napi::Uint32Array, size_t)                  \
  V(Napi::Int32Array, size_t)                   \

#define DECLARE_TYPED_ARRAY_MAPPING_FROM_C_TO_NATIVE(C_TYPE, N_TYPE, SUBTYPE)       \
  template<>                                             \
  struct MAPPING_TO_NATIVE<C_TYPE> {                       \
    using type = N_TYPE;                                 \
    static bool check(const Napi::Value& value) {        \
      return value.IsTypedArray() &&                     \
        value.As<Napi::TypedArray>().TypedArrayType() == \
        SUBTYPE;                                         \
    }                                                    \
  };

TYPED_ARRAY_MAPPING_LIST(DECLARE_TYPED_ARRAY_MAPPING_FROM_C_TO_NATIVE);
#undef DECLARE_TYPED_ARRAY_MAPPING_FROM_C_TONATIVE

#define DECLARE_PRIMITIVE_MAPPING_FROM_C_TO_NATIVE(C_TYPE, N_TYPE, CHECK) \
  template<>                                                         \
  struct MAPPING_TO_NATIVE<C_TYPE> {                                   \
    using type = N_TYPE;                                             \
    static bool check(const Napi::Value& value) {                    \
      return value.CHECK();                                          \
    }                                                                \
  };
PRIMITIVE_TYPE_MAPPING_LIST(DECLARE_PRIMITIVE_MAPPING_FROM_C_TO_NATIVE)
#undef DECLARE_PRIMITIVE_MAPPING_FROM_C_TO_NATIVE

template<typename T>
struct SizeTypeMapping {
  using type = void;
};
#define DECLARE_TYPED_ARRAY_MAPPING_FROM_C_TONATIVE_FROM_NATIVE_TO_CTYPE(NT, CT) \
  template<>                                    \
  struct SizeTypeMapping<NT> {                  \
    using type = CT;                            \
  };

TYPED_ARRAY_SIZE_TYPE_MAPPING_LIST(DECLARE_TYPED_ARRAY_MAPPING_FROM_C_TONATIVE_FROM_NATIVE_TO_CTYPE);
#undef DECLARE_TYPED_ARRAY_MAPPING_FROM_C_TONATIVE_FROM_NATIVE_TO_CTYPE

namespace async = EVSPACE::ASYNC;
template<typename T>
struct AsyncStructMapping {
  static_assert(std::integral_constant<bool, false>::value,
    "Not an async structure");
};
template<typename T>
struct AsyncStructMapping<async::RingPipeMeta<T>> {
  static Napi::Value notify_one(const Napi::CallbackInfo& info) {
    if (!info[0].IsObject()) {
      Napi::Error::New(info.Env(), "Wrong argument");
      return info.Env().Undefined();
    }

    Napi::Object obj = info[0].As<Napi::Object>();
    if (!obj.Has("pipe_addr")) {
      Napi::Error::New(info.Env(), "Wrong argument");
      return info.Env().Undefined();
    }

    auto addr = obj.Get("pipe_addr");
    Napi::Number addr_num = addr.As<Napi::Number>();

    async::RingPipeMeta<T>* pipe = reinterpret_cast<async::RingPipeMeta<T>*>(
      addr_num.Int64Value());

    return info.Env().Undefined();
  }

  static Napi::Value notify_all(const Napi::CallbackInfo& info) {

  }

  static Napi::Value mapping(async::RingPipeMeta<T>& meta,
                             const Napi::CallbackInfo& info) {
    auto object = Napi::Object::New(info.Env());
    // TODO: AsyncStruct itself does not has any information to
    // about the type of what message should oppsite-side should used.
    object.Set("message_type", Napi::String::New(info.Env(), "Counter"));

    Napi::ArrayBuffer pipe_buffer = Napi::ArrayBuffer::New(
      info.Env(), meta.pipe, meta.length);
    Napi::Uint8Array array = Napi::Uint8Array
      ::New(info.Env(),
            meta.length,
            pipe_buffer, 0);
    object.Set("pipe", array);

    Napi::ArrayBuffer rw_head_buffer = Napi::ArrayBuffer::New(
      info.Env(), meta.rw_head, sizeof(uint32_t) * 2);
    Napi::Uint32Array rw_head = Napi::Uint32Array
      ::New(info.Env(), 2, rw_head_buffer, 0);
    object.Set("rw_head", rw_head);
    object.Set("length", Napi::Number::New(info.Env(), meta.length));

    ASSERT(sizeof(meta.instance) == sizeof(int64_t));
    object.Set("pipe_addr", Napi::Number::New(info.Env(), meta.instance));

    object.Set("notify_one", Napi::Function::New<notify_one>(info.Env()));
    object.Set("notify_all", Napi::Function::New<notify_all>(info.Env()));

    return object;
  }

};
template<typename T>
struct AsyncStructMapping<EVSPACE::ASYNC::SymPipeMeta<T>> {
  static Napi::Value mapping(
    EVSPACE::ASYNC::SymPipeMeta<T>& meta,
    const Napi::CallbackInfo& info) {

    auto object = Napi::Object::New(info.Env());
    object.Set("in", AsyncStructMapping<ASYNC::RingPipeMeta<T>>
               ::mapping(meta.out_meta, info));
    object.Set("out", AsyncStructMapping<ASYNC::RingPipeMeta<T>>
               ::mapping(meta.in_meta, info));
    return object;
  }
};


template<typename... Ts>
struct Count {
  constexpr static size_t count = 0;
};
template<typename T, typename... Ts>
struct Count<T, Ts...> {
  constexpr static size_t count = 1 + Count<Ts...>::count;
};


template<typename... Ts>
struct Tlist {
  using type = std::tuple<>;
};
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
template<typename T, typename... Ts>
struct TAtPos<0, T, Ts...> {
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

template<typename R, typename... Ts>
struct Reduce { using type = R; };
template<template<typename...> class R, typename... Os, typename T, typename... Ts>
struct Reduce<R<Os...>, T, Ts...> {
  using type = typename Reduce<R<Os..., typename MAPPING_TO_NATIVE<T>::type>, Ts...>::type;
};
template<template<typename...> class R, typename... Os, typename T, typename U, typename... Ts>
struct Reduce<R<Os...>, T*, U, Ts...> {
  // Declaration Layer Interface must declare parameter to indicate
  // valid range of a pointer at the pos next to where pointer parameter
  // is declared. Other wise treat as not-well form.
  //
  // Typically, the type U to indicate valid ranges should be size_t
  static_assert(std::is_same_v<U, size_t>,
                "Parameter to indicate valid range in Memory "
                "of pointer must at pos next to the pointer");
  // U is ignored due to such message are passed by Napi::TypeArray<T>.
  using NType = typename MAPPING_TO_NATIVE<T*>::type;
  using type = typename Reduce<R<Os..., NType>, Ts...>::type;
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
using NodeTypes = typename Reduce<std::tuple<>, Ts...>::type;
template<typename... Ts>
std::optional<NodeTypes<Ts...>>
AsNatives(const Napi::CallbackInfo& info) {
  using inferNativeType = typename Reduce<
    std::tuple<>, Ts...>::type;

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
struct DeclareArg<T, ArgIDX<I>> {
  using type = T;
  constexpr static size_t index = I;
};

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
  using InferType = typename MAPPING_TO_NATIVE<T*>::type;
  static_assert(std::is_same_v<TheNodeType, InferType>);
  static_assert(std::is_same_v<U, size_t>);

  using type = typename Declaring<
    pos+1,
    std::tuple<
      Os...,
      DeclareArg<T*,ArgIDX<pos>>,
      DeclareArg<U ,ArgIDX<pos>>>,
    Tlist<NTs...>,
    Ts...>
    ::type;
};
template<size_t pos, typename... Os, typename T, typename... Ts, typename... NTs>
struct Declaring<pos, std::tuple<Os...>, Tlist<NTs...>, T, Ts...> {
  // Must to gurantee that Napi-Type inference from T* match the type
  // we need to extract information out.
  using TheNodeType = typename GetTlist<pos,Tlist<NTs...>>::type;
  using InferType = typename MAPPING_TO_NATIVE<T>::type;
  static_assert(std::is_same_v<TheNodeType, InferType>);

  using type = typename Declaring<
    pos + 1,
    std::tuple<
      Os...,
      DeclareArg<T,ArgIDX<pos>>>,
    Tlist<NTs...>,
    Ts...>
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
  V(Napi::Float64Array , size_t , ElementLength)

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
struct ArgExtracter;
template<typename... Ts>
struct ArgExtracter<std::tuple<Ts...>> {
  template<typename Arg>
  static auto argTrans(std::tuple<Ts...>& t) {
    using trait = DeclareArg_Trait<Arg>;
    using ArgType = typename trait::type;
    using SelectedNativeType = typename GetTlist<trait::pos, Tlist<Ts...>>::type;
    using NativeReductT = typename MAPPING_TO_NATIVE<ArgType>::type;
    using SizeType = typename SizeTypeMapping<SelectedNativeType>::type;

    static_assert(trait::pos < Count<Ts...>::count, "");
    static_assert(std::is_same_v<NativeReductT, SelectedNativeType> ||
                  /* otherwise, it should be a size_t  */
                  (std::is_same_v<ArgType, size_t> &&
                   std::is_same_v<SizeType, size_t>));

    return NativeTranslator<
      SelectedNativeType,
      typename trait::type>::trans(std::get<trait::pos>(t));
  }
};

template<typename FN, typename... Args>
struct AsNodeExpression;
template<typename... Args, typename... ExtractInfos, typename... Ts>
struct AsNodeExpression<void(*)(Args...),
                        std::tuple<ExtractInfos...>,
                        std::tuple<Ts...>> {

  static_assert((std::is_same_v<Args, typename ExtractInfos::type> && ...));

  static auto eval(void(*f)(Args...),
                   std::tuple<Ts...>& t,
                   const Napi::CallbackInfo& info) {
    f(ArgExtracter<std::tuple<Ts...>>::template argTrans<ExtractInfos>(t)...);
    return info.Env().Undefined();
  }
};
template<typename T, typename... ExtractInfos, typename... Ts>
struct AsNodeExpression<EVSPACE::ASYNC::SymPipeMeta<T>(*)(),
                        std::tuple<ExtractInfos...>,
                        std::tuple<Ts...>> {
  static auto eval(EVSPACE::ASYNC::SymPipeMeta<T>(*f)(),
                   std::tuple<Ts...>& t,
                   const Napi::CallbackInfo& info) {
    using Meta = ASYNC::SymPipeMeta<T>;
    auto pipe = f(ArgExtracter<std::tuple<Ts...>>::template argTrans<ExtractInfos>(t)...);
    return AsyncStructMapping<Meta>::mapping(pipe, info);
  }
};
template<typename R, typename... Args, typename... ExtractInfos, typename... Ts>
struct AsNodeExpression<R(*)(Args...),
                        std::tuple<ExtractInfos...>,
                        std::tuple<Ts...>> {

  static_assert((std::is_same_v<Args, typename ExtractInfos::type> && ...));

  static auto eval(R(*f)(Args...),
                   std::tuple<Ts...>& t,
                   const Napi::CallbackInfo& info) {
    return f(ArgExtracter<std::tuple<Ts...>>::template argTrans<ExtractInfos>(t)...);
  }
};

template<typename FN, typename... Ts>
struct EvalFlat;
template<typename FN, typename... FN_ARGs, typename... Ts>
struct EvalFlat<FN, std::tuple<FN_ARGs...>, std::tuple<Ts...>> {
  static auto eval(FN fn, std::tuple<Ts...>& t,
                   const Napi::CallbackInfo& info) {
    using ArgInfos = typename Declaring<
      0, Tlist<FN_ARGs...>, Tlist<Ts...>>::type;

    return AsNodeExpression<FN, ArgInfos, std::tuple<Ts...>>::eval(fn, t, info);
  }
};

template<typename FN, typename... Ts>
auto Eval(FN fn, NodeTypes<Ts...> t, const Napi::CallbackInfo& info) {
  static_assert(std::is_invocable_v<FN, Ts...>);
  return EvalFlat<FN, std::tuple<Ts...>, NodeTypes<Ts...>>::eval(fn, t, info);
}

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
#define PARAM_TRANS_IMPL(...)                           \
  GET_MACRO_5(          \
    _0,                 \
    ##__VA_ARGS__,      \
    PARAM_TRANS_IMPL_N, \
    PARAM_TRANS_IMPL_N, \
    PARAM_TRANS_IMPL_N, \
    PARAM_TRANS_IMPL_N, \
    PARAM_TRANS_IMPL_N, \
    PARAM_TRANS_IMPL_0)

#define EVAL_WITH_ARGS(B, R, ...) \
  return CODEGEN::Eval<R(*)(__VA_ARGS__), __VA_ARGS__>( \
    EVSPACE::BASIS::DECL::B, NATIVE_TUPLE_RECEIVER.value(), info)
#define EVAL_WITHOUT_ARGS(B, R, ...) \
  return CODEGEN::Eval<R(*)()>( \
    EVSPACE::BASIS::DECL::B, std::tuple{}, info)
#define EVAL(B, R, ...)       \
  GET_MACRO_5(                \
    _0,                       \
    ##__VA_ARGS__,            \
    EVAL_WITH_ARGS,           \
    EVAL_WITH_ARGS,            \
    EVAL_WTIH_ARGS,            \
    EVAL_WITH_ARGS,            \
    EVAL_WITH_ARGS,                             \
    EVAL_WITHOUT_ARGS)(B, R, __VA_ARGS__)


#define NODE_NATIVE_WRAPPER_PARAM_TRANS(        \
  DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE)     \
  PARA_NAME(PARAM_TRANS_IMPL)(                  \
    DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE)   \

#define NODE_NATIVE_WRAPPER_EVAL(DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE) \
  PARA_TYPE(EVAL);

#define SPAWN_NODE_NATIVE_WRAPPER_SIGNATURE(    \
  DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE, IS_C_LINKAGE)     \
  Napi::Value _##DECLARE(const Napi::CallbackInfo& info) {              \
    NODE_NATIVE_WRAPPER_PARAM_TRANS(DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE); \
    NODE_NATIVE_WRAPPER_EVAL(DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE); \
  }

#define REGISTERS(DECLARE, SIGNATURE, PARA_NAME, PARA_TYPE, IS_C_LINKAGE) \
  exports[#DECLARE] = Napi::Function::New(env, _##DECLARE);
#define SPAWN_API_REGISTER_BLOCK() \
  static Napi::Object Init(Napi::Env env, Napi::Object exports) {       \
    DECLARED_BASISES(REGISTERS);                                        \
    return exports;                                                     \
  }                                                                     \
  NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init);


} // CODGEN
} // NODE_NATIVE_SPACE
} // SPACE
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_SPACES_NODE_NATIVE_SPACE_CODEGEN_H_ */
