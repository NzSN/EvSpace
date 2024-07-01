#include "base.h"
#include "napi.h"


namespace EVAL {
namespace EVALS {
namespace NATIVES {

Napi::Value _PolyFunction(const Napi::CallbackInfo& info);
Napi::Value _PthreadTest(const Napi::CallbackInfo& info);

} // NATIVES
} // EVALS
} // EVAL
