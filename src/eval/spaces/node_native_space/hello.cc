#define NAPI_DISABLE_CPP_EXCEPTIONS
#include <napi.h>
#include "eval/basis/basis_declarations.h"
#include "node_native_space.h"

namespace EVSPACE {
namespace EVAL {
namespace SPACE {
namespace NODE_NATIVE_SPACE {

DECLARED_BASISES(SPAN_NODE_NATIVE_SPACE_FROM_BASIS)

Napi::Value _Sort(const Napi::CallbackInfo& info) {
  return info.Env().Undefined();
}

static Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports["example"] = Napi::Function::New(env, _Sort);
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init);


} // NODE_NATIVE_SPACE
} // SPACE
} // EVAL
} // EVSPACE
