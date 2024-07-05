#include "../basis.h"

#ifndef EVSPACE_EVAL_BASIS_SORT_SORTH_H_
#define EVSPACE_EVAL_BASIS_SORT_SORTH_H_

/* Those Basis that expected to publish globally
 * need be listed here otherwise those Basis
 * is not registered into any non-trivial spaces. */
#define DECLARED_SORT_BASIS(V)                                          \
  V(InsertionSort, EVSPACE::EVAL::BASIS::SORT, \
    void, double*, size_t,)


#endif /* EVSPACE_EVAL_BASIS_SORT_SORTH_H_ */
