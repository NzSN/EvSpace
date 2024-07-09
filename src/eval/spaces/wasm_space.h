#ifndef EVSPACE_EVAL_SPACE_H_
#define EVSPACE_EVAL_SPACE_H_

#include <vector>

#include "eval/basis/sort/sort.h"
#include "eval/basis/basis_declarations.h"


/* Due to BASIS are declared in a form that 'Parametric Polymorphism'
 * is in used and there is no convinient ways to do that for C API so
 * transformation from BASIS to C_API required an process to collapse
 * those type variables into concrete type. */
template<typename T, typename... Ts>
struct C_TYPE_REDUCTION {
  using type = T; /* Types that already defined in C */
};
template<>
struct C_TYPE_REDUCTION<std::string> {
  using type = char*;
};
template<typename T>
struct C_TYPE_REDUCTION<std::vector<T>> {
  using type = C_TYPE_REDUCTION<T>::type;
};

#define C_API_FROM_BASIS(BASIS)                 \
  C_TYPE_REDUCTION<RETURN_TYPE_OF_BASIS(BASIS)> \
  BASIS (PARAMETER_TYPES_OF_BASIS(BASIS));



namespace EVSPACE::EVAL::SPACE::WASM_SPACE {

#define SPAN_WASM_SPACE_FROM_BASIS(BASIS) \


} // EVSPACE::EVAL::SPACE::WASM_SPACE

#endif /* EVSPACE_EVAL_SPACE_H_ */
