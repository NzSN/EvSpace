#include "base.h"
#include "napi.h"

namespace EVAL {
namespace EVALS {
namespace NATIVES {

Napi::Value _Sort(const Napi::CallbackInfo& info);
Napi::Value _SortWithDebug(const Napi::CallbackInfo& info);

} // NATIVES
} // EVALS
} // EVAL
