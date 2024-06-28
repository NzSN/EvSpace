#include <napi.h>
#include "js_native_api_types.h"
#include <assert.h>
#include <cmath>
#include <math.h>
#include <optional>

namespace Eval {

template<typename T>
struct TypedArray {
  T* data;
  size_t length;
};

static std::optional<TypedArray<double>>
AcceptFloat64Array(const Napi::CallbackInfo& info) {
  if (info.Length() != 1) {
    return std::nullopt;
  }

  if (!info[0].IsTypedArray()) {
    return std::nullopt;
  }

  Napi::TypedArray typedArray = info[0].As<Napi::TypedArray>();
  if (typedArray.TypedArrayType() != napi_float64_array) {
    return std::nullopt;
  }
  Napi::Float64Array float64Array = typedArray.As<Napi::Float64Array>();

  return TypedArray<double>{
    float64Array.Data(),
    float64Array.ElementLength() };
}

double PolyFunction(TypedArray<double> coefficients) {
  double result = 0;
  for (size_t i = 0; i < coefficients.length; ++i) {
    result += coefficients.data[i] * pow(1, 2);
  }

  return result;
}

static Napi::Value _PolyFunction(const Napi::CallbackInfo& info) {
  auto coefficients_opt = AcceptFloat64Array(info);
  if (!coefficients_opt.has_value()) {
    Napi::Error::New(info.Env(), "Wrong arguments");
    return info.Env().Undefined();
  }
  double polyVal = PolyFunction(coefficients_opt.value());

  return Napi::Number::New(info.Env(), polyVal);
}

static Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports["PolyFunction"] = Napi::Function::New(env, _PolyFunction);

  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init);

} // Eval
