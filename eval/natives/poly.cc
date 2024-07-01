#include <napi.h>
#include "js_native_api_types.h"
#include <assert.h>
#include <cmath>
#include <math.h>
#include <optional>

#include "base.h"
#include "poly.h"
#include "../evals/poly.h"


namespace EVAL {
namespace EVALS {
namespace NATIVES {

double PolyFunction(BASE::TypedArray<double> coefficients) {
  return EvalPolyFunc(coefficients.data, coefficients.length, 1);
}

Napi::Value _PolyFunction(const Napi::CallbackInfo& info) {
  auto coefficients_opt = BASE::AcceptFloat64Array(info);
  if (!coefficients_opt.has_value()) {
    Napi::Error::New(info.Env(), "Wrong arguments");
    return info.Env().Undefined();
  }
  double polyVal = PolyFunction(coefficients_opt.value());

  return Napi::Number::New(info.Env(), polyVal);
}

double PthreadTest() {
  return EVAL::EVALS::PthreadTest();
}

Napi::Value _PthreadTest(const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), PthreadTest());
}

} // NATIVES
} // EVALS
} // Eval
