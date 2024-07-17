#include <gtest/gtest.h>
#include <rapidcheck/gtest.h>

#include "insertion_sort.h"

namespace EVSPACE::EVAL::BASIS::SORT {


RC_GTEST_PROP(Tester, BasisSpec, ()) {
  InsertionSort<double> sort;
  SortBasis<double> *g_sort = &sort;

  std::vector<double> vec { 5, 4, 3, 2, 1, 0 };

  (*g_sort)(&(*vec.begin()), vec.size());

  for (size_t i = 1; i < vec.size(); ++i) {
    RC_ASSERT(vec[i] > vec[i-1]);
  }

}


} // EVSPACE::EVAL::BASIS:SORT
