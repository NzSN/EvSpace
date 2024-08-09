#ifndef EVSPACE_EVAL_ASYNC_H_
#define EVSPACE_EVAL_ASYNC_H_

#include <thread>

#include "eval/async/traits.h"
#include "eval/async/messages/messages.h"
#include "eval/async/control/control.h"
#include "eval/async/transfer/pipe.h"

namespace EVSPACE {
namespace ASYNC {

#define ASYNCHRONOUS(T) EVSPACE::ASYNC::TaskEnvMeta<T>

#define ASYNC_PARAM(T) EVSPACE::ASYNC::BiPipeParam<T,T>
#define INIT_ASYNC_STRUCTURE(T, PARAM)                              \
  ([](ASYNC_PARAM(T)& param) -> auto {                              \
    auto env = std::make_unique<EVSPACE::ASYNC::TaskEnv<T>>(PARAM); \
    ASYNCHRONOUS(T) meta = env->generateMeta();                     \
    return std::tuple(meta, std::move(env));                        \
  })(PARAM)
#define ENV_PTR(T) std::unique_ptr<EVSPACE::ASYNC::TaskEnv<T>>
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

template<typename T>
struct TaskEnvMeta {
  BiPipeMeta<T,T> pipe_meta;
  ControlMeta control_meta;
};

template<typename T>
struct TaskEnv {
  TaskEnv(ASYNC_PARAM(T) param):
    control{}, pipe{param} {}

  TaskEnvMeta<T> generateMeta() {
    TaskEnvMeta<T> meta = {
      .pipe_meta = pipe.generateMeta(),
      .control_meta = control.generateMeta(),
    };

    return meta;
  }

  Control control;
  EVSPACE::ASYNC::BiPipe<T,T> pipe;
};

template<typename T>
struct Task {
  virtual void operator()(ENV_PTR(T)) = 0;
};

} // ASYNC
} // EVSPACE


#endif /* EVSPACE_EVAL_ASYNC_H_ */
