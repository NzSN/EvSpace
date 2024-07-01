#include "poly.h"
#include "sort.h"

namespace EVAL {
namespace EVALS {
namespace NATIVES {

static Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports["PolyFunction"] = Napi::Function::New(env, _PolyFunction);
  exports["Sort"] = Napi::Function::New(env, _Sort);
  exports["SortWithDebug"] = Napi::Function::New(env, _SortWithDebug);
  exports["PthreadTest"] = Napi::Function::New(env, _PthreadTest);

  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init);

} // NATIVES
} // EVALS
} // EVAL
