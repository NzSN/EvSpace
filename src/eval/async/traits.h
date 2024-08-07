#ifndef EVSPACE_EVAL_ASYNC_TRAITS_H_
#define EVSPACE_EVAL_ASYNC_TRAITS_H_

#include "base/traits.h"

namespace EVSPACE {
namespace ASYNC {

DEFINE_TRAIT_HAS_METHOD(ByteSizeLong);
DEFINE_TRAIT_HAS_METHOD(SerializeToArray);
DEFINE_TRAIT_HAS_METHOD(ParseFromArray);


} // ASYNC
} // EVSPACE

#endif /* EVSPACE_EVAL_ASYNC_TRAITS_H_ */
