#include "eval/basis/basis.h"
#include "insertion_sort.h"

#ifndef EVSPACE_EVAL_BASIS_SORT_SORTH_H_
#define EVSPACE_EVAL_BASIS_SORT_SORTH_H_

/* Wrap your basis with a C interface so it's able to
 * be used as seed to generate different functions
 * among spaces */
extern "C" {
  void insertionSort(double *seq, size_t size);
  void trivialSort(double *seq, size_t size, double *seq_2, size_t size_2);
}

/* Those Basis that expected to publish globally
 * need be listed here otherwise those Basis
 * is not registered into any non-trivial spaces. */
#define DECLARED_SORT_BASISES(V)                \
  V(insertionSort,                              \
    insertionSignature,                         \
    insertionParaName,                          \
    insertionTypes)                             \
  V(trivialSort,                                \
    trivialSortSignature,                       \
    trivialSortParaName,                        \
    trivialSortTypes)

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
#define trivialSortSignature(V) \
  V(trivialSort, void, double* seq, size_t size, double* seq1, size_t size1)
#define trivialSortParaName(V) \
  V(seq, size, seq1, size1)
#define trivialSortTypes(V) \
  V(trivialSort, void, double*, size_t, double*, size_t)

#endif /* EVSPACE_EVAL_BASIS_SORT_SORTH_H_ */
