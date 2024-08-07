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
      auto msg = pipe->read();
      if (msg.has_value()) {
        if (msg.value().counter() == 0) {
          return;
        } else {
          while (!pipe->writable()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
          }
          counter.set_counter(msg.value().counter());
          pipe->write(counter);
        }
      }
    }
  }

  DEFINE_SYMMETRIC_ASYNC_INTERFACE(
    T, operator(), Echo::DoEcho, ([]{ return 10; })());
};

} // MISC
} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_MISC_TRIVIAL_H_ */
