#ifndef EVSPACE_BASE_UTILITY_H_
#define EVSPACE_BASE_UTILITY_H_

#include <random>

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
