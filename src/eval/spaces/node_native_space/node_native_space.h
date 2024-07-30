#ifndef EVSPACE_EVAL_SPACES_NODE_NATIVE_SPACE_H_
#define EVSPACE_EVAL_SPACES_NODE_NATIVE_SPACE_H_

#include "space_generator.h"

namespace EVSPACE {
namespace EVAL {
namespace SPACE {
namespace NODE_NATIVE_SPACE {

#define SPAN_NODE_NATIVE_SPACE_FROM_BASIS(BASIS, SIGNATURE, PARA_NAME, PARA_TYPE, IS_C_LINKAGE) \
  SPAWN_NODE_NATIVE_WRAPPER_SIGNATURE(BASIS, SIGNATURE, PARA_NAME, PARA_TYPE, IS_C_LINKAGE)

} // NODE_NATIVE_SPACE
} // SPACE
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_SPACES_NODE_NATIVE_S PACE_H_ */
