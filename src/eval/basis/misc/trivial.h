#ifndef EVSPACE_EVAL_BASIS_MISC_TRIVIAL_H_
#define EVSPACE_EVAL_BASIS_MISC_TRIVIAL_H_

#include <thread>
#include <chrono>

#include "eval/async/messages/counter.pb.h"
#include "eval/async/pipe.h"
#include "eval/basis/misc/misc.h"

namespace EVSPACE {
namespace EVAL {
namespace BASIS {
namespace MISC {

namespace async = ::EVSPACE::ASYNC;

template<typename T>
struct Echo : AsyncBasis<T> {
  static void DoEcho(async::BiPipe<Counter,Counter> pipe) {
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

    async::BiPipeParam<T,T> param = {
      .in_message = &counter,
      .in_length = 10,
      .out_message = &counter,
      .out_length = 10,
    };

    async::BiPipe<T,T> pipe(param);
    ASYNCHRONOUS(T) meta = pipe.generateMeta();

    std::thread t0{Echo::DoEcho, std::move(pipe)};
    t0.detach();

    return meta;
  }

};

} // MISC
} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_MISC_TRIVIAL_H_ */
