#include "base.h"

namespace EVAL::EVALS::NATIVES::BASE {

std::optional<TypedArray<double>>
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


} // EVAL::EVALS::NATIVES::BASE
