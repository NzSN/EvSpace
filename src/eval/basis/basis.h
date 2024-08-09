#ifndef EVSPACE_EVAL_BASIS_BASIS_H_
#define EVSPACE_EVAL_BASIS_BASIS_H_

namespace EVSPACE {
namespace EVAL {
namespace BASIS {

#define TRUE_M(V)  V(0,1)
#define FALSE_M(V) V(0)

#define DECL_SIGNATURE(BASIS) BASIS##Signature
#define DECL_PARA_IDENT(BASIS) BASIS##ParaName
#define DECL_SIGNATURE_TYPE_ONLY(BASIS) BASIS##Types

struct Basis {};

} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_BASIS_H_ */
