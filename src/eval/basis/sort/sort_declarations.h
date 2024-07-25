#include "eval/basis/basis.h"
#include "insertion_sort.h"
#include "eval/basis/async_pipe.h"

#ifndef EVSPACE_EVAL_BASIS_SORT_SORTH_H_
#define EVSPACE_EVAL_BASIS_SORT_SORTH_H_

/* Wrap your basis with a C interface so it's able to
 * be used as seed to generate different functions
 * among spaces */
extern "C" {
  void insertionSort(double *seq, size_t size);

  /* Counting from 0 to N, N is specified by seq[2],
   * on seq[0] with delay specified by seq[1] */
  ASYNC_PIPE(void) asyncCount(double *seq, size_t size);
}

/* Those Basis that expected to publish globally
 * need be listed here otherwise those Basis
 * is not registered into any non-trivial spaces. */
#define DECLARED_SORT_BASISES(V)                \
  V(insertionSort,                              \
    insertionSignature,                         \
    insertionParaName,                          \
    insertionTypes)                             \
  V(asyncCount,                                \
    asyncCountSignature,                       \
    asyncCountParaName,                        \
    asyncCountTypes)

///////////////////////////////////////////////////////////////////////////////
//                             InsertionSort Meta                            //
///////////////////////////////////////////////////////////////////////////////
#define insertionSignature(V)                   \
  V(insertionSort, void, double* seq, size_t size)
#define insertionParaName(V)                    \
  V(seq, size)
#define insertionTypes(V)                       \
  V(insertionSort, void, double*, size_t)

///////////////////////////////////////////////////////////////////////////////
//                              TrivialSort meta                             //
///////////////////////////////////////////////////////////////////////////////
#define asyncCountSignature(V) \
  V(asyncCount, void, double* seq, size_t size)
#define asyncCountParaName(V) \
  V(seq, size)
#define asyncCountTypes(V) \
  V(asyncCount, void, double*, size_t)

#endif /* EVSPACE_EVAL_BASIS_SORT_SORTH_H_ */
