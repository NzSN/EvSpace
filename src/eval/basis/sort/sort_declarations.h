#include "../basis.h"
#include "insertion_sort.h"

#ifndef EVSPACE_EVAL_BASIS_SORT_SORTH_H_
#define EVSPACE_EVAL_BASIS_SORT_SORTH_H_

namespace EVSPACE {
namespace EVAL {
namespace BASIS {
namespace SORT {

/* Those Basis that expected to publish globally
 * need be listed here otherwise those Basis
 * is not registered into any non-trivial spaces. */
#define DECLARED_SORT_BASISES(V)                \
  V(InsertionSort)

///////////////////////////////////////////////////////////////////////////////
//                                Basis Metas                                //
///////////////////////////////////////////////////////////////////////////////
#define InsertionTemplateTypeArguments(V) \
  V(OrderableAndSpanble T, typename S = std::span<T>>)
#define InsertionArgumentTypes(V) \
  V(S&)
#define InsertionArguments(V) \
  V(seq)
#define InsertionReturnType(V) \
  V(void)


} // SORT
} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_SORT_SORTH_H_ */
