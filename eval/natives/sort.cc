#include "sort.h"
#include "../evals/sort.h"

namespace EVAL {
namespace EVALS {
namespace NATIVES {

void Sort(BASE::TypedArray<double> nums) {
  EVAL::EVALS::Sort(nums.data, nums.length);
}

double SortWithDebug(BASE::TypedArray<double> nums) {
  return EVAL::EVALS::SortWithDebug(nums.data, nums.length);
}


Napi::Value _Sort(const Napi::CallbackInfo& info) {
  auto nums_opt = BASE::AcceptFloat64Array(info);
  if (!nums_opt.has_value()) {
    Napi::Error::New(info.Env(), "Wrong argument");
    return info.Env().Undefined();
  }

  Sort(nums_opt.value());

  return info.Env().Undefined();
}

Napi::Value _SortWithDebug(const Napi::CallbackInfo& info) {
  auto nums_opt = BASE::AcceptFloat64Array(info);
  if (!nums_opt.has_value()) {
    Napi::Error::New(info.Env(), "Wrong argument");
    return info.Env().Undefined();
  }

  return Napi::Number::New(info.Env(), SortWithDebug(nums_opt.value()));
}


} // WASM
} // EVALS
} // EVAL
