#include "eval/basis/basis.h"
#include "insertion_sort.h"

#ifndef EVSPACE_EVAL_BASIS_SORT_SORTH_H_
#define EVSPACE_EVAL_BASIS_SORT_SORTH_H_

/* Wrap your basis with a C interface so it's able to
 * be used as seed to generate different functions
 * among spaces */
extern "C" {
  void insertionSort(double *seq, size_t size);
}

namespace EVSPACE {
namespace EVAL {
namespace BASIS {
namespace SORT {

/* Those Basis that expected to publish globally
 * need be listed here otherwise those Basis
 * is not registered into any non-trivial spaces. */
#define DECLARED_SORT_BASISES(V)                \
  V(insertionSort,                              \
    insertionSignature,                         \
    insertionParaName,                          \
    insertionTypes)

///////////////////////////////////////////////////////////////////////////////
//                             InsertionSort Meta                            //
///////////////////////////////////////////////////////////////////////////////
#define insertionSignature(V)                   \
  V(insertionSort, void, double* seq, size_t size)
#define insertionParaName(V)                    \
  V(seq, size)
#define insertionTypes(V)                       \
  V(void, double*, size_t)

} // SORT
} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_SORT_SORTH_H_ */
