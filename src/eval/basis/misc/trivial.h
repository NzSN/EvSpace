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
  static void DoEcho(PIPE(T) pipe) {
    Counter counter;
    while (true) {
      if (pipe.readable()) {
        auto msg = pipe.read();
        if (msg.has_value()) {
          if (msg.value().counter() == 0) {
            return;
          } else {
            while (!pipe.writable()) {
              std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            counter.set_counter(msg.value().counter());
            pipe.write(counter);
          }
        }
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }


  ASYNCHRONOUS(T) operator()() {
    Counter counter;
    counter.set_counter(1);
    ASYNC_PARAM(T) param = {
      .in_message = &counter,
      .in_length = 10,
      .out_message = &counter,
      .out_length = 10,
    };

    auto meta = INIT_ASYNC_STRUCTURE(T, param);
    BOOTING_ASYNC_THREAD(Echo::DoEcho, std::move(AS_BOOT_ARGS(meta)));

    return AS_JS_META(meta);
  }

};

} // MISC
} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_MISC_TRIVIAL_H_ */
