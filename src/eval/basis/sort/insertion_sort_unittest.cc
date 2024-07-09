#include <gtest/gtest.h>
#include <rapidcheck/gtest.h>

#include "eval/basis/basis.h"
#include "eval/basis/sort/insertion_sort.h"
#include "eval/basis/sort/sort.h"

namespace EVSPACE::EVAL::BASIS::SORT {

template<typename Elem,
         template<typename _Elem, typename = std::span<_Elem>> class T>
requires(requires (T<Elem>& t) { SortBasisUnit<Elem>{&t}; })
struct SortBasisTester : public ::testing::Test {

  decltype(auto) GetSort() {
    resource = std::make_unique<T<Elem>>();

    return SortBasisUnit<Elem>{resource.get()};
  }

  std::unique_ptr<T<Elem>> resource;
};

using InsertionTester = SortBasisTester<double, InsertionSort>;
RC_GTEST_FIXTURE_PROP(InsertionTester, InsertionSort, ()) {
  SortBasisUnit<double> sort = GetSort();

  std::vector<double> vec { 5, 4, 3, 2, 1, 0 };
  std::span<double> s(vec);

  (*sort)(s);

  for (size_t i = 1; i < vec.size(); ++i) {
    RC_ASSERT(vec[i] > vec[i-1]);
  }
}



} // EVSPACE::EVAL::BASIS:SORT
