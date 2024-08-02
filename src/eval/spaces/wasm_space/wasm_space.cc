#include <cstdint>
#include <thread>
#include "wasm_space.h"
#include "eval/async/pipe.h"
#include <chrono>
#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "eval/async/messages/messages.h"
#include "eval/async/messages/counter.pb.h"

template<>
struct em::smart_ptr_trait<std::unique_ptr<uint8_t>> {
  typedef typename std::unique_ptr<uint8_t>::element_type element_type;
  static uint8_t* get(const std::unique_ptr<uint8_t>& ptr) {
    return ptr.get();
  }
};

namespace EVSPACE {
namespace EVAL {
namespace SPACE {
namespace WASM_SPACE {
namespace CODEGEN {

#define DECLARE_PIPE_META(MSG)                                    \
  em::class_<EmPipeMeta<MSG>>("EmPipeMeta")                   \
    .property("message_type", &EmPipeMeta<MSG>::message_type) \
    .property("pipe", &EmPipeMeta<MSG>::pipe)                 \
    .property("rw_head", &EmPipeMeta<MSG>::rw_head)           \
    .property("length", &EmPipeMeta<MSG>::length);            \
                                                                  \
  em::class_<EmBiPipeMeta<MSG,MSG>>("EmBiPipeMeta")       \
    .property("in", &EmBiPipeMeta<MSG, MSG>::out_meta)    \
    .property("out", &EmBiPipeMeta<MSG, MSG>::in_meta);


DECLARED_BASISES(SPAN_WASM_SPACE_FROM_BASIS);

EMSCRIPTEN_BINDINGS(WasmSpace) {
  DECLARED_BASISES(SPAWN_EMBINDING);
  DECLARED_MESSAGES(DECLARE_PIPE_META);
}

} // CODEGEN
} // WASM_SPACE
} // SPACE
} // EVAL
} // EVSPACE
