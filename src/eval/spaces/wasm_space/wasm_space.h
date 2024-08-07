#ifndef EVSPACE_EVAL_SPACE_H_
#define EVSPACE_EVAL_SPACE_H_

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "eval/async/async.h"
#include "eval/async/messages/messages.h"
#include "eval/basis/basis_declarations.h"

#include "base/utility.h"

namespace em = emscripten;
namespace async = EVSPACE::ASYNC;


namespace EVSPACE {
namespace EVAL {
namespace SPACE {
namespace WASM_SPACE {
namespace CODEGEN {

#define DECLARE_PIPE_META(MSG)                                \
  em::class_<EmPipeMeta<MSG>>("EmPipeMeta")                   \
  .property("message_type", &EmPipeMeta<MSG>::message_type)   \
  .property("pipe", &EmPipeMeta<MSG>::pipe)                   \
  .property("rw_head", &EmPipeMeta<MSG>::rw_head)             \
  .property("length", &EmPipeMeta<MSG>::length)               \
  .function("notify_one", &EmPipeMeta<MSG>::notify_one)       \
  .function("notify_all", &EmPipeMeta<MSG>::notify_all);      \
                                                              \
  em::class_<EmBiPipeMeta<MSG,MSG>>("EmBiPipeMeta")           \
  .property("in", &EmBiPipeMeta<MSG, MSG>::out_meta)          \
  .property("out", &EmBiPipeMeta<MSG, MSG>::in_meta);         \
                                                              \
  em::class_<EmControlMeta>("EmControlMeta");                 \
                                                              \
  em::class_<EmTaskEnvMeta<MSG>>("EmTaskEnvMeta")             \
  .property("control", &EmTaskEnvMeta<MSG>::control_meta)     \
  .property("pipe", &EmTaskEnvMeta<MSG>::pipe_meta);




template<typename T>
struct EmPipeMeta {
  EmPipeMeta(async::RingPipe<T>& pipe):
    message_type{async::MESSAGE::MessageType<T>::msg_type},
    pipe(em::typed_memory_view(pipe.length(), pipe.pipe())),
    rw_head{em::typed_memory_view(2, pipe.rw_head_.get())},
    length{pipe.length()},
    instance{&pipe} {}
  EmPipeMeta(async::RingPipeMeta<T>& meta):
    message_type{async::MESSAGE::MessageType<T>::msg_type},
    pipe{em::typed_memory_view(meta.length, meta.pipe)},
    rw_head{em::typed_memory_view(2, meta.rw_head)},
    length{meta.length},
    instance{meta.instance} {}

  void notify_one() {
    instance->notify_one();
  }

  void notify_all() {
    instance->notify_all();
  }

  std::string message_type;
  em::val pipe;
  em::val rw_head;
  size_t length;

  async::RingPipe<T>* instance;
};

template<typename T, typename U>
struct EmBiPipeMeta {
  EmBiPipeMeta(async::BiPipe<T, U>& pipe):
    in_meta{async::MESSAGE::MessageType<T>::msg_type, pipe.in_},
    out_meta{async::MESSAGE::MessageType<U>::msg_type, pipe.out_} {}
  EmBiPipeMeta(async::BiPipeMeta<T, U>& meta):
    in_meta{async::MESSAGE::MessageType<T>::msg_type, meta.in_meta},
    out_meta{async::MESSAGE::MessageType<U>::msg_type, meta.out_meta} {}

  EmPipeMeta<T> in_meta;
  EmPipeMeta<U> out_meta;
};

struct EmControlMeta {

};

template<typename T>
struct EmTaskEnvMeta {
  EmTaskEnvMeta(async::TaskEnvMeta<T> env):
    control_meta{}, pipe_meta{env.pipe_meta} {}

  EmControlMeta control_meta;
  EmBiPipeMeta<T,T> pipe_meta;
};


template<typename T>
struct TypeMapping {
  using type = T;
};
template<typename T>
struct TypeMapping<async::RingPipeMeta<T>> {
  using type = EmPipeMeta<T>;
};
template<typename T, typename U>
struct TypeMapping<async::BiPipeMeta<T, U>> {
  using type = EmBiPipeMeta<T, U>;
};

template<typename T>
struct Conversion {
  static T conversion(T& t) {
    return t;
  }
};
template<typename T>
struct Conversion<async::RingPipeMeta<T>> {
  static typename TypeMapping<async::RingPipeMeta<T>>::type
  conversion(async::RingPipeMeta<T>& meta) {
    return EmPipeMeta<T>(meta);
  }
};

template<typename T, typename U>
struct Conversion<async::BiPipeMeta<T,U>> {
  static typename TypeMapping<async::BiPipeMeta<T, U>>::type
  conversion(async::BiPipeMeta<T, U>& meta) {
    return EmBiPipeMeta<T,U>(meta);
  }
};

template<typename...>
struct CALLING;
template<typename R, typename... ARGS>
struct CALLING<R(*)(ARGS...)> {
  static typename TypeMapping<R>::type call(R(*f)(ARGS...), ARGS... args) {
    auto meta = f(args...);
    return Conversion<R>::conversion(meta);
  }
};
template<typename... ARGS>
struct CALLING<void(*)(ARGS...)> {
  static void call(void(*f)(ARGS...), ARGS... args) {
    f(args...);
  }
};

#define GEN_SIGNATURE(BASIS, R, ...)                                \
  EMSCRIPTEN_KEEPALIVE TypeMapping<R>::type BASIS##w (__VA_ARGS__)
#define PRINT_PARA_0(...)
#define PRINT_PARA_N(...) , __VA_ARGS__
#define PRINT_PARA(...)                         \
  GET_MACRO_5(                                  \
    _0,                                         \
    ##__VA_ARGS__,                              \
    PRINT_PARA_N,                               \
    PRINT_PARA_N,                               \
    PRINT_PARA_N,                               \
    PRINT_PARA_N,                               \
    PRINT_PARA_N,                               \
    PRINT_PARA_0)(__VA_ARGS__)
#define PRINT_ARG_TYPES(B, R, ...) __VA_ARGS__
#define PRINT_F_TYPE(B, R, ...) R(*)(__VA_ARGS__)
#define AS_WASM_BASIS_EMBIND(B, SIGNATURE, PARA_NAME, TYPES)       \
  SIGNATURE(GEN_SIGNATURE) {                                       \
    return CALLING<TYPES(PRINT_F_TYPE)>::call(                     \
      EVSPACE::BASIS::DECL::B PARA_NAME(PRINT_PARA));              \
  }
#define AS_WASM_BASIS_C_LINKAGE(B, SIGNATURE, PARA_NAME, TYPES)         \
  extern "C" { SIGNATURE(GEN_SIGNATURE); }                              \
  SIGNATURE(GEN_SIGNATURE) {                                            \
    return CALLING<TYPES(PRINT_F_TYPE)>::call(                          \
      EVSPACE::BASIS::DECL::B /* , is added by PRINT_PARA is nedded */  \
      PARA_NAME(PRINT_PARA));                                           \
  }
#define WRAP_BASIS(...)                                             \
  GET_MACRO_2(                                                      \
    _0,                                                             \
    ##__VA_ARGS__,                                                  \
    AS_WASM_BASIS_C_LINKAGE,                                        \
    AS_WASM_BASIS_EMBIND)
#define AS_WASM_BASIS(B, SIGNATURE, PARA_NAME, TYPES, IS_C_LINKAGE) \
  IS_C_LINKAGE(WRAP_BASIS)(B, SIGNATURE, PARA_NAME, TYPES)

#define SPAN_WASM_SPACE_FROM_BASIS(BASIS, R, ...) \
  AS_WASM_BASIS(BASIS, R, __VA_ARGS__)

#define NO_EMBINDING(BASIS)
#define DECLARE_EMBINDING(BASIS)                \
  em::function(#BASIS, &BASIS##w);
#define CHOOSE_EMBINDING_MACRO(...)                               \
  GET_MACRO_2(_0, ##__VA_ARGS__, NO_EMBINDING, DECLARE_EMBINDING)
#define DECLARE_EMBINDING_IF_NOT_C_LINKAGE(BASIS, IS_C_LINKAGE) \
  IS_C_LINKAGE(CHOOSE_EMBINDING_MACRO)(BASIS)
#define SPAWN_EMBINDING(BASIS, SIGNATURE, PARA_NAME, TYPES, IS_C_LINKAGE) \
  DECLARE_EMBINDING_IF_NOT_C_LINKAGE(BASIS, IS_C_LINKAGE)

} // CODEGEN
} // WASM_SPACE
} // SPACE
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_SPACE_H_ */
