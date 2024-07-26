#ifndef LANTR_BASE_UTILITY_ASSERT_H_
#define LANTR_BASE_UTILITY_ASSERT_H_

#include <iostream>

#ifdef NDEBUG
#define ASSERT(COND, MSG)
#else

#if __APPLE__
#include <TargetConditionals.h>

#if TARGET_OS_MAC

#define ASSERT_FAILURE_MESSAGE(COND)              \
  "Assertion failed with condition"               \
  << #COND                                        \
  << " at "                                       \
  << std::string{__FILE__} + ":" + std::to_string(__LINE__)
#endif

#else

#define ASSERT_FAILURE_MESSAGE(COND)                        \
  "Assertion failed with condition"                         \
  << #COND                                                  \
  << " at "                                                 \
  << std::string{__FILE__} + ":" + std::to_string(__LINE__)


#endif

#define ASSERT_WITHOUT_MSG(COND)                            \
  do {                                                      \
    if (!(COND)) {                                          \
      std::cerr << ASSERT_FAILURE_MESSAGE(COND);            \
      std::abort();                                         \
    }                                                       \
  } while(0);

#define ASSERT_WITH_MSG(COND, MSG)                \
  do {                                            \
    if (!(COND)) {                                \
      std::cerr << ASSERT_FAILURE_MESSAGE(COND);  \
      std::abort();                               \
    }                                             \
  } while (0);

#define GET_MACRO(_1,_2,NAME,...) NAME
#define ASSERT(...) GET_MACRO( \
    __VA_ARGS__,               \
    ASSERT_WITH_MSG,           \
    ASSERT_WITHOUT_MSG)(__VA_ARGS__)

#ifdef NO_SLOW_ASSERT
#define ASSERT_SLOW(COND, MSG)
#else

#define ASSERT_SLOW_WITH_MSG(COND, MSG) ASSERT(COND, MSG)
#define ASSERT_SLOW_WITHOUT_MSG(COND) ASSERT(COND)

#define ASSERT_SLOW(...) GET_MACRO( \
    __VA_ARGS__,                    \
    ASSERT_SLOW_WITH_MSG,           \
    ASSERT_SLOW_WITHOUT_MSG)(__VA_ARGS__)

#endif // NO_SLOW_ASSERT

#endif // NDEBUG

#endif // LANTR_BASE_UTILITY_ASSERT_H_
