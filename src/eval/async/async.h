#ifndef EVSPACE_EVAL_ASYNC_H_
#define EVSPACE_EVAL_ASYNC_H_

#include "eval/async/pipe.h"

namespace EVSPACE {
namespace ASYNC {

#define ASYNCHRONOUS(T) EVSPACE::ASYNC::SymPipeMeta<T>

#define ASYNC_PARAM(T) EVSPACE::ASYNC::BiPipeParam<T,T>
#define INIT_ASYNC_STRUCTURE(T, PARAM)                                \
  ([](ASYNC_PARAM(T)& param) -> auto {                                \
    auto pipe = std::make_unique<EVSPACE::ASYNC::BiPipe<T,T>>(PARAM); \
    ASYNCHRONOUS(T) meta = pipe->generateMeta();                       \
    return std::tuple(meta, std::move(pipe));                         \
  })(PARAM)
#define PIPE(T) std::unique_ptr<EVSPACE::ASYNC::BiPipe<T,T>>
#define AS_BOOT_ARGS(META) std::get<1>(META)
#define AS_JS_META(META) std::get<0>(META)

#define BOOTING_ASYNC_THREAD(F, ...)                    \
  std::thread __evspace_async_thread{F, __VA_ARGS__};   \
  __evspace_async_thread.detach();

#define DEFINE_SYMMETRIC_ASYNC_INTERFACE(T, IDENT, IMPL, Q_LEN) \
  ASYNCHRONOUS(T) IDENT() {                                            \
    ASYNC_PARAM(T) param = {                                           \
      .in_length = Q_LEN,                                              \
      .out_length = Q_LEN,                                             \
    };                                                                 \
    auto meta = INIT_ASYNC_STRUCTURE(T, param);                        \
    BOOTING_ASYNC_THREAD(IMPL, std::move(AS_BOOT_ARGS(meta)));         \
    return AS_JS_META(meta);                                           \
  }


} // ASYNC
} // EVSPACE


#endif /* EVSPACE_EVAL_ASYNC_H_ */
