#include <napi.h>
#include <stddef.h>
#include <optional>

#ifndef EVAL_EVALS_NATIVES_BASE_H_
#define EVAL_EVALS_NATIVES_BASE_H_

namespace EVAL::EVALS::NATIVES::BASE {

template<typename T>
struct TypedArray {
  T* data;
  size_t length;
};

std::optional<TypedArray<double>>
AcceptFloat64Array(const Napi::CallbackInfo& info);

} // EVAL::EVALS::NATIVES

#endif /* EVAL_EVALS_NATIVES_BASE_H_ */
