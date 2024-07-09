#ifndef EVSPACE_EVAL_BASIS_BASIS_H_
#define EVSPACE_EVAL_BASIS_BASIS_H_

#include "proxy/proxy.h"

namespace EVSPACE {
namespace EVAL {
namespace BASIS {

/* BasisUnit  */
PRO_DEF_OPERATOR_DISPATCH(Basis, "()");

template<typename... Overloads>
struct BasisCallable :
    pro::facade_builder
    ::add_convention<Basis, Overloads...>
    ::build {};

struct CopyableCallable :
    pro::facade_builder
    ::support_copy<pro::constraint_level::nontrivial>
    ::build {};

struct Destructable :
    pro::facade_builder
    ::support_destruction<pro::constraint_level::nontrivial>
    ::build {};

template<typename... Overloads>
struct BasisUnit :
    pro::facade_builder
    ::add_facade<BasisCallable<Overloads...>>
    ::template add_facade<CopyableCallable>
    ::template add_facade<Destructable>
    ::build {};

template<typename... Overloads>
using ProBasis = pro::proxy<BasisUnit<Overloads...>>;

} // BASIS
} // EVAL
} // EVSPACE

#endif /* EVSPACE_EVAL_BASIS_BASIS_H_ */
