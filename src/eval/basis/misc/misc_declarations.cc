#include "misc_declarations.h"
#include "eval/async/async.h"
#include "eval/basis/misc/trivial.h"

namespace EVSPACE {
namespace BASIS {
namespace DECL {

ASYNCHRONOUS(Counter) Echo() {
  return EVSPACE::EVAL::BASIS::MISC::Echo<Counter>{}();
}

} // DECL
} // BASIS
} // EVSPACE
