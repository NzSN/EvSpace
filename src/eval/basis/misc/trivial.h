#ifndef EVSPACE_EVAL_BASIS_MISC_TRIVIAL_H_
#define EVSPACE_EVAL_BASIS_MISC_TRIVIAL_H_

#include <thread>
#include <chrono>

#include "eval/async/messages/counter.pb.h"
#include "eval/async/async.h"
#include "eval/basis/misc/misc.h"

namespace EVSPACE {
namespace EVAL {
namespace BASIS {
namespace MISC {

namespace async = ::EVSPACE::ASYNC;

template<typename T>
struct Echo : AsyncBasis<T> {

  struct EchoTask: public ASYNC::Task<T> {
    void operator()(ENV_PTR(T) env) override {
      Counter counter;
      while (true) {
        auto msg = env->pipe.read();
        if (msg.has_value()) {
          if (msg.value().counter() == 0) {
            return;
          } else {
            counter.set_counter(msg.value().counter());
            env->pipe.write(counter);
          }
        }
      }
    }
  };

  DEFINE_SYMMETRIC_ASYNC_INTERFACE(
    T, operator(), Echo::EchoTask{}, ([]{ return 64; })());
};

} // MISC
} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_MISC_TRIVIAL_H_ */
