#include "eval/basis/basis.h"
#include "trivial.h"
#include "eval/async/pipe.h"
#include "eval/async/messages/counter.pb.h"

#ifndef EVSPACE_EVAL_BASIS_MISC_MISC_DECLARATIONS_H_
#define EVSPACE_EVAL_BASIS_MISC_MISC_DECLARATIONS_H_

namespace EVSPACE {
namespace BASIS {
namespace DECL {

#define ECHO_SIGN DECL_SIGNATURE(Echo)
#define ECHO_PARAS DECL_PARA_IDENT(Echo)
#define ECHO_TYPES_ONLY DECL_SIGNATURE_TYPE_ONLY(Echo)
ASYNCHRONOUS(Counter) Echo();


#define DECLARED_MISC_BASISES(V) \
  V(Echo, ECHO_SIGN, ECHO_PARAS, ECHO_TYPES_ONLY)
#define EMBIND_MISC_BASISES(V) \
  V(Echo)

///////////////////////////////////////////////////////////////////////////////
//                                   Metas                                   //
///////////////////////////////////////////////////////////////////////////////
#define EchoSignature(V) \
  V(Echo, ASYNCHRONOUS(Counter))
#define EchoParaName(V)  \
  V()
#define EchoTypes(V)     \
  V(Echo, ASYNCHRONOUS(Counter))


} // DECL
} // BASIS
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_MISC_MISC_DECLARATIONS_H_ */
