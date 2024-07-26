#include <cstdint>
#include <thread>
#include "wasm_space.h"
#include "eval/async/pipe.h"
#include <chrono>
#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "eval/async/messages/counter.pb.h"

// DECLARED_BASISES(SPAN_WASM_SPACE_FROM_BASIS);

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
struct EmPipe {
  EmPipe(std::string msg_type,
         async::MessagePipe<T>& pipe):
    message_type(msg_type),
    pipe(em::typed_memory_view(pipe.size(), pipe.pipe())),
    length{pipe.length()} {}

  std::string message_type;
  em::val pipe;
  size_t length;
};

void F(async::MessagePipe<Counter> pipe) {
  Counter counter;

  counter.set_counter(0);
  counter.SerializeToArray(pipe.pipe(), pipe.size());

  while (counter.counter() < 60) {
    counter.set_counter(counter.counter() + 1);
    counter.SerializeToArray(pipe.pipe(), pipe.size());
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void G(async::MessagePipe<Counter> pipe) {
  std::vector<size_t> counterBounds;

  while (true) {
   
  }
}

template<typename T>
EmPipe<T> Communication() {
  Counter counter;
  counter.set_counter(1);

  async::MessagePipe<Counter>
    pipe(counter, 1);

  EmPipe<T> emPipe("Counter", pipe);

  std::thread t0{F, std::move(pipe)};
  t0.detach();

  return emPipe;
}

template<typename T>
EmPipe<T> MultiSettableCounter() {
  Counter counter;
  counter.set_counter(1);

  async::MessagePipe<T> pipe(counter, 3);

  EmPipe<T> emPipe("Counter", pipe);

  std::thread t0{G, std::move(pipe)};
  t0.detach();

  return emPipe;
}

EMSCRIPTEN_BINDINGS(WasmSpace) {
  em::function("Communication", &Communication<Counter>);
  em::function("MultiSettableCounter", &MultiSettableCounter<Counter>);

  em::class_<EmPipe<Counter>>("EmPipe")
    .property("message_type", &EmPipe<Counter>::message_type)
    .property("pipe", &EmPipe<Counter>::pipe)
    .property("length", &EmPipe<Counter>::length);
}
