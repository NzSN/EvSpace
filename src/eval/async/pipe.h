#ifndef EVSPACE_EVAL_ASYNC_PIPE_H_
#define EVSPACE_EVAL_ASYNC_PIPE_H_

#include <mutex>
#include <cstdint>
#include <memory>
#include <stddef.h>
#include <optional>

#include "base/assert.h"
#include "base/thread_safety.h"
#include "base/traits.h"

#define ASYNCHRONOUS EVSPACE::ASYNC::RingPipe

namespace EVSPACE {
namespace ASYNC {


struct PipeTester;
DEFINE_TRAIT_HAS_METHOD(ByteSizeLong);
DEFINE_TRAIT_HAS_METHOD(SerializeToArray);
DEFINE_TRAIT_HAS_METHOD(ParseFromArray);

template<typename T,
         typename = std::enable_if<
           has_method_ByteSizeLong<T,size_t()>::value &&
           has_method_SerializeToArray<T, bool(void*,size_t)>::value &&
           has_method_ParseFromArray<T, bool(const void*, size_t)>::value>>
class RingPipe {
public:
  RingPipe(const T& message, size_t length):
    pipe_{std::make_unique<uint8_t[]>(message.ByteSizeLong() * length)},
    head_{pipe_.get()},
    last_{pipe_.get()},
    begin_{pipe_.get()},
    end_{pipe_.get() + message.ByteSizeLong() * length},
    size_{message.ByteSizeLong()},
    length_{length} {}

  RingPipe(const RingPipe&) = delete;
  RingPipe& operator=(RingPipe&) = delete;

  RingPipe(RingPipe&& other):
    pipe_{std::move(other.pipe_)},
    size_{other.size_},
    length_{other.length_} {}

  RingPipe& operator=(RingPipe&& other) {
    this->pipe_ = std::move(other.pipe_);
    this->size_ = other.size_;
    this->length_ = other.length_;

    return *this;
  }

  size_t size() const {
    return size_;
  }

  size_t length() const {
    return length_;
  }

  uint8_t* pipe() {
    return pipe_.get();
  }

  std::optional<T> read() EXCLUDES(last_mutex_) {
    std::lock_guard<std::mutex> lock{last_mutex_};

    if (isEmpty()) {
      return std::nullopt;
    }

    T message;
    message.ParseFromArray(last_, size_);
    last_ = next(last_);

    return message;
  }

  std::optional<T> peek() EXCLUDES(last_mutex_) {
    std::lock_guard<std::mutex> lock{last_mutex_};
    if (isEmpty()) {
      return std::nullopt;
    }

    T message;
    message.ParseFromArray(last_, size_);

    return message;
  }

  bool write(const T& message) EXCLUDES(head_mutex_) {
    std::lock_guard<std::mutex> lock{head_mutex_};

    if (isFull()) {
      return false;
    }

    message.SerializeToArray(head_, size_);
    head_ = next(head_);
    return true;
  }

  bool isEmpty() const NO_THREAD_SAFETY_ANALYSIS {
    return head_ == last_;
  }

  bool isFull() const NO_THREAD_SAFETY_ANALYSIS {
    return length_ == 0 || next(head_) == last_;
  }

private:
  friend PipeTester;

  uint8_t* next(uint8_t* current) const {
    ASSERT(begin_ <= current && current < end_,
           "Invalid address");
    uint8_t* next_pos = current + size_;
    if (next_pos >= end_) {
      return const_cast<uint8_t*>(begin_);
    } else {
      return next_pos;
    }
  }

  std::unique_ptr<uint8_t[]> pipe_;
  uint8_t* head_ GUARDED_BY(head_mutex_);
  uint8_t* last_ GUARDED_BY(last_mutex_);

  const uint8_t* begin_;
  const uint8_t* end_;

  // Size of message in pipe
  size_t size_;
  // Number of messages in pipe
  size_t length_;

  std::mutex head_mutex_;
  std::mutex last_mutex_;
};

template<typename T, typename U>
struct BiPipeParam {
  const T* in_message;
  size_t   in_length;

  const T* out_message;
  size_t   out_length;
};

template<typename T, typename U>
class BiPipe {
public:
  BiPipe(BiPipeParam<T,U>& param):
    in_ {*param.in_message, param.in_length},
    out_{*param.out_message, param.out_length} {}

  bool write(const T& message) {
    return out_.write(message);
  }

  std::optional<T> read() {
    return in_.read();
  }

  std::optional<T> peek() {
    return in_.peek();
  }

  bool readable() const {
    return !in_.isEmpty();
  }
  bool writable() const {
    return !out_.isFull();
  }

private:
  RingPipe<T> in_;
  RingPipe<U> out_;
};

} // ASYNC
} // EVSPACE

#endif /* EVSPACE_EVAL_ASYNC_PIPE_H_ */
