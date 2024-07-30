#ifndef EVSPACE_EVAL_BASIS_BASIS_H_
#define EVSPACE_EVAL_BASIS_BASIS_H_

#include <type_traits>

namespace EVSPACE {
namespace EVAL {
namespace BASIS {

#define DECL_SIGNATURE(BASIS) BASIS##Signature
#define DECL_PARA_IDENT(BASIS) BASIS##ParaName
#define DECL_SIGNATURE_TYPE_ONLY(BASIS) BASIS##Types

struct Basis {};

} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_BASIS_H_ */
