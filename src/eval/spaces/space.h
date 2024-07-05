#ifndef EVSPACE_EVAL_SPACES_SPACE_H_
#define EVSPACE_EVAL_SPACES_SPACE_H_

#include <stddef.h>

#include "base/macros.h"
#include "eval/basis/basis.h"
#include "eval/basis/basis_declarations.h"

#include "eval/basis/sort/insertion_sort.h"

namespace EVSPACE {
namespace EVAL {
namespace SPACE {

#define SPAN_FROM_BASIS(BASIS, NSS, R, ...)   \
  R BASIS_F(__VA_ARGS__) {                    \
    return NSS::BASIS(__VA_ARGS__);           \
  }


struct Space {};

struct GlobalSpace: public Space {
  DECLARED_BASIS(SPAN_FROM_BASIS)
};

struct SortSpace: public Space {
  DECLARED_SORT_BASIS(SPAN_FROM_BASIS)
};

} // SPACE
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_SPACES_SPACE_H_ */
