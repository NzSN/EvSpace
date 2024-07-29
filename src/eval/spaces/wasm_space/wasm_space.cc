#include <cstdint>
#include <thread>
#include "wasm_space.h"
#include "eval/async/pipe.h"
#include <chrono>
#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "eval/async/messages/counter.pb.h"

DECLARED_BASISES(SPAN_WASM_SPACE_FROM_BASIS);

namespace em = emscripten;
namespace async = EVSPACE::ASYNC;

template<>
struct em::smart_ptr_trait<std::unique_ptr<uint8_t>> {
  typedef typename std::unique_ptr<uint8_t>::element_type element_type;
  static uint8_t* get(const std::unique_ptr<uint8_t>& ptr) {
    return ptr.get();
  }
};

namespace BASIS {

ASYNCHRONOUS<Counter> Communication();

} // BASIS

template<typename T>
struct EmPipeMeta {
  EmPipeMeta(std::string msg_type, async::RingPipe<T>& pipe):
    message_type(msg_type),
    pipe(em::typed_memory_view(pipe.length() * pipe.msgSize(), pipe.pipe())),
    rw_head{em::typed_memory_view(
        sizeof(pipe.rw_head_) / sizeof(uint32_t), pipe.rw_head_)},
    length{pipe.length()},
    msgSize{pipe.msgSize()} {}

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

  EmPipeMeta<T> in_meta;
  EmPipeMeta<U> out_meta;
};

void Communication_fn(async::RingPipe<Counter> pipe) {
  Counter counter;

  counter.set_counter(0);
  counter.SerializeToArray(pipe.pipe(), pipe.msgSize());

  while (counter.counter() < 60) {
    counter.set_counter(counter.counter() + 1);
    counter.SerializeToArray(pipe.pipe(), pipe.msgSize());
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void Echo_fn(async::BiPipe<Counter,Counter> pipe) {
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

          Counter counter;
          counter.set_counter(msg.value().counter());
          pipe.write(counter);
        }
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

template<typename T>
EmPipeMeta<T> Communication() {
  Counter counter;
  counter.set_counter(1);

  async::RingPipe<Counter>
    pipe(counter, 1);

  EmPipeMeta<T> emPipe("Counter", pipe);

  std::thread t0{Communication_fn, std::move(pipe)};
  t0.detach();

  return emPipe;
}

template<typename T>
EmBiPipeMeta<T,T> Echo() {
  Counter counter;
  counter.set_counter(1);

  async::BiPipeParam<T,T> param = {
    .in_message = &counter,
    .in_length = 10,
    .out_message = &counter,
    .out_length = 10,
  };
  async::BiPipe<T, T> pipe(param);
  EmBiPipeMeta<T,T> pipeMeta("Counter", "Counter", pipe);

  std::thread t0{Echo_fn, std::move(pipe)};
  t0.detach();

  return pipeMeta;
}

EMSCRIPTEN_BINDINGS(WasmSpace) {
  em::function("Communication", &Communication<Counter>);
  em::function("Echo",  &Echo<Counter>);

  em::class_<EmPipeMeta<Counter>>("EmPipeMeta")
    .property("message_type", &EmPipeMeta<Counter>::message_type)
    .property("pipe", &EmPipeMeta<Counter>::pipe)
    .property("rw_head", &EmPipeMeta<Counter>::rw_head)
    .property("length", &EmPipeMeta<Counter>::length)
    .property("size", &EmPipeMeta<Counter>::msgSize);

  em::class_<EmBiPipeMeta<Counter,Counter>>("EmBiPipeMeta")
    .property("in", &EmBiPipeMeta<Counter, Counter>::out_meta)
    .property("out", &EmBiPipeMeta<Counter, Counter>::in_meta);
}
