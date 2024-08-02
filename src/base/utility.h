#ifndef EVSPACE_BASE_UTILITY_H_
#define EVSPACE_BASE_UTILITY_H_

#include <random>

#define GET_MACRO_1(_0, _1, _NAME, ...) _NAME
#define GET_MACRO_2(_0, _1, _2, _NAME, ...) _NAME
#define GET_MACRO_3(_0, _1, _2, _3, _NAME, ...) _NAME
#define GET_MACRO_4(_0, _1, _2, _3, _4, _NAME, ...) _NAME
#define GET_MACRO_5(_0, _1, _2, _3, _4, _5, _NAME, ...) _NAME

#define IS_ALIGNED(ADDRESS, BYTE_COUNT) \
    (((uintptr_t)(const void *)(ADDRESS)) % (BYTE_COUNT) == 0)

namespace EVSPACE {
namespace BASE {
namespace UTILITY {

template<size_t min, size_t max>
struct Random {
  size_t operator()() {
    if (!inited) {
      rng = std::mt19937(dev());
      dist = std::uniform_int_distribution<
        std::mt19937::result_type>(min,max);
    }

    return dist(rng);
  }

  static bool inited;
  static std::random_device dev;
  static std::mt19937 rng;
  static std::uniform_int_distribution<std::mt19937::result_type> dist;
};

template<size_t min, size_t max>
bool Random<min,max>::inited = false;
template<size_t min, size_t max>
std::random_device Random<min,max>::dev;
template<size_t min, size_t max>
std::mt19937 Random<min,max>::rng;
template<size_t min, size_t max>
std::uniform_int_distribution<std::mt19937::result_type>
Random<min,max>::dist;

} // UTILITY
} // BASE
} // EVSPACE


#endif /* EVSPACE_BASE_UTILITY_H_ */
