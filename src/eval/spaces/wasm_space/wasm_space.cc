#include "wasm_space.h"
#include <emscripten/bind.h>

using namespace emscripten;
namespace DECL = EVSPACE::BASIS::DECL;

EMSCRIPTEN_BINDINGS(WasmSpace) {
  function("insertionSort", &DECL::insertionSort, allow_raw_pointers());
  function("asyncCount", &DECL::asyncCount, allow_raw_pointers());
}
