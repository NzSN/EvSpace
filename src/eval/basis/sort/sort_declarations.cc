#include <chrono>
#include <cmath>
#include <thread>
#include <pthread.h>

#include "sort_declarations.h"
#include "eval/basis/sort/insertion_sort.h"

namespace SORT = EVSPACE::EVAL::BASIS::SORT;

namespace EVSPACE{
namespace BASIS {
namespace DECL {

void insertionSort(double *seq, size_t size) {
  SORT::InsertionSort<double>{}(seq,size);
}

void fn(double* seq, size_t size) {
  if (size < 2) {
    return;
  }

  seq[0] = 0;
  while (true) {
    if (seq[0] > 60) break;
    std::this_thread::sleep_for(
      std::chrono::milliseconds(std::lround(seq[1])));
    seq[0]++;
  }

}
void asyncCount(double* seq, size_t size) {
  std::thread t0{fn, seq, size};
  t0.detach();
}


} // DECL
} // BASIS
} // EVSPACE
