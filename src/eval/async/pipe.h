#ifndef EVSPACE_EVAL_ASYNC_PIPE_H_
#define EVSPACE_EVAL_ASYNC_PIPE_H_

#include <iostream>
#include <memory>
#include <type_traits>
#include "stddef.h"

#define ASYNCHRONOUS EVSPACE::ASYNC::MessagePipe

namespace EVSPACE {
namespace ASYNC {

template<typename T, typename F>
struct has_method {
  static_assert(std::integral_constant<F, false>::value,
    "The second parameter of has_method<T,F> must be a function type");
};

#define HAS_METHOD(T, M, R) \
  template<typename T, typename R, typename... Args> \
  struct has_method<T, R(Args...)> {                 \
    template<typename U>                             \
    static constexpr auto check(U*) -> typename      \
      std::is_same<decltype(std::declval<U>().M())>     \
  };

template<typename T>
struct is_message {
private:
  static constexpr auto check(T*) -> typename
    std::is_same<decltype(std::declval<T>().ByteSizeLong()), size_t>::type;
  static constexpr std::false_type check(...);

  typedef decltype(check(0)) type;
public:
  static constexpr bool value = type::value;
};

template<typename T,
         std::enable_if<is_message<T>::value>>
class MessagePipe {
public:
  MessagePipe(T& message, size_t length):
    pipe_{std::make_unique<uint8_t>(message.ByteSizeLong() * length)},
    size_{message.ByteSizeLong()},
    length_{length} {}

  MessagePipe(const MessagePipe&) = delete;

  MessagePipe(MessagePipe&& other):
    pipe_{std::move(other.pipe_)},
    size_{other.size_},
    length_{other.length_} {}

  size_t size() const {
    return size_;
  }

  size_t length() const {
    return length_;
  }

  uint8_t* pipe() {
    return pipe_.get();
  }



private:
  std::unique_ptr<uint8_t> pipe_;
  // Size of message in pipe
  size_t size_;
  // Number of messages in pipe
  size_t length_;
};

} // ASYNC
} // EVSPACE

#endif /* EVSPACE_EVAL_ASYNC_PIPE_H_ */
