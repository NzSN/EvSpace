#ifndef EVSPACE_EVAL_SPACE_H_
#define EVSPACE_EVAL_SPACE_H_

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "eval/async/pipe.h"
#include "eval/basis/basis_declarations.h"

namespace em = emscripten;
namespace async = EVSPACE::ASYNC;


namespace EVSPACE {
namespace EVAL {
namespace SPACE {
namespace WASM_SPACE {
namespace CODEGEN {

template<typename T>
struct EmPipeMeta {
  EmPipeMeta(std::string msg_type, async::RingPipe<T>& pipe):
    message_type(msg_type),
    pipe(em::typed_memory_view(pipe.length() * pipe.msgSize(), pipe.pipe())),
    rw_head{em::typed_memory_view(2, pipe.rw_head_.get())},
    length{pipe.length()},
    msgSize{pipe.msgSize()} {}
  EmPipeMeta(std::string msg_type, async::RingPipeMeta<T>& meta):
    message_type(msg_type),
    pipe{em::typed_memory_view(meta.length * meta.msgSize, meta.pipe)},
    rw_head{em::typed_memory_view(2, meta.rw_head)},
    length{meta.length},
    msgSize{meta.msgSize} {}

  std::string message_type;
  em::val pipe;
  em::val rw_head;
  size_t length;
  size_t msgSize;
};

template<typename T, typename U>
struct EmBiPipeMeta {
  EmBiPipeMeta(std::string in_msg_type, std::string out_msg_type,
               async::BiPipe<T, U>& pipe):
    in_meta{in_msg_type, pipe.in_},
    out_meta{out_msg_type, pipe.out_} {}
  EmBiPipeMeta(std::string in_msg_type, std::string out_msg_type,
    async::BiPipeMeta<T, U>& meta):
    in_meta{in_msg_type, meta.in_meta},
    out_meta{out_msg_type, meta.out_meta} {}

  EmPipeMeta<T> in_meta;
  EmPipeMeta<U> out_meta;
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
    return EmPipeMeta<T>("Counter", meta);
  }
};

template<typename T, typename U>
struct Conversion<async::BiPipeMeta<T,U>> {
  static typename TypeMapping<async::BiPipeMeta<T, U>>::type
  conversion(async::BiPipeMeta<T, U>& meta) {
    return EmBiPipeMeta<T,U>("Counter", "Counter", meta);
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
#define CHOOSE_PRINT_PARA_N(_0, _1, _2, _3, _4, _5, _Name, ...) _Name
#define PRINT_PARA(...)                         \
  CHOOSE_PRINT_PARA_N(                          \
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
#define AS_WASM_BASIS(B, SIGNATURE, PARA_NAME, TYPES) \
  SIGNATURE(GEN_SIGNATURE) {                              \
    return CALLING<TYPES(PRINT_F_TYPE)>::call( \
      EVSPACE::BASIS::DECL::B PARA_NAME(PRINT_PARA));              \
  }
#define AS_WASM_BASIS_C_LINKAGE(B, SIGNATURE, PARA_NAME, TYPES) \
  extern "C" { SIGNATURE(GEN_SIGNATURE); }                \
  SIGNATURE(GEN_SIGNATURE) {                              \
    return CALLING<TYPES(PRINT_F_TYPE)>::call( \
      EVSPACE::BASIS::DECL::B PARA_NAME(PRINT_PARA));              \
  }


#define SPAN_WASM_SPACE_FROM_BASIS(BASIS, R, ...) \
  AS_WASM_BASIS(BASIS, R, __VA_ARGS__)

#define SPAWN_EMBINDING(BASIS) \
  em::function(#BASIS, &BASIS##w);

} // CODEGEN
} // WASM_SPACE
} // SPACE
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_SPACE_H_ */
