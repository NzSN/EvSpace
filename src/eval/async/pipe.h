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
#include "base/utility.h"

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

#define ASSERT_IDX_VALIDATION(IDX) \
  ASSERT(begin_ <= IDX && IDX < end_, "INVALID PIPE IDX")

  RingPipe(const T& message, size_t length):
    pipe_{std::make_unique<uint8_t[]>(message.ByteSizeLong() * length)},
    head_{},
    read_idx_{&head_[0]},
    write_idx_{&head_[1]},
    begin_{0},
    end_{length},
    begin_addr_{pipe_.get()},
    end_addr_{pipe_.get() + message.ByteSizeLong() * length},
    size_{message.ByteSizeLong()},
    length_{length} {

    ASSERT(IS_ALIGNED(pipe_.get(), size_));
  }

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

  std::optional<T> read() EXCLUDES(read_mutex_) {
    std::lock_guard<std::mutex> lock{read_mutex_};

    if (isEmpty()) {
      return std::nullopt;
    }

    ASSERT_IDX_VALIDATION(*read_idx_);

    uint8_t* slot_to_read = addrOfIdx(*read_idx_);

    T message;
    message.ParseFromArray(slot_to_read, size_);
    *read_idx_ = next(*read_idx_);

    return message;
  }

  std::optional<T> peek() EXCLUDES(read_mutex_) {
    std::lock_guard<std::mutex> lock{read_mutex_};
    if (isEmpty()) {
      return std::nullopt;
    }

    ASSERT_IDX_VALIDATION(*read_idx_);

    uint8_t* slot_to_peek = addrOfIdx(*read_idx_);

    T message;
    message.ParseFromArray(slot_to_peek, size_);

    return message;
  }

  bool write(const T& message) EXCLUDES(write_mutex_) {
    std::lock_guard<std::mutex> lock{write_mutex_};

    if (isFull()) {
      return false;
    }

    ASSERT_IDX_VALIDATION(*write_idx_)

      uint8_t* slot_to_write = addrOfIdx(*write_idx_);

    message.SerializeToArray(slot_to_write, size_);
    *write_idx_ = next(*write_idx_);
    return true;
  }

  bool isEmpty() const NO_THREAD_SAFETY_ANALYSIS {
    return *read_idx_ == *write_idx_;
  }

  bool isFull() const NO_THREAD_SAFETY_ANALYSIS {
    return length_ == 0 || next(*write_idx_) == *read_idx_;
  }

  uint8_t* addrOfIdx(uint32_t idx) {
    return const_cast<uint8_t*>(begin_addr_ + (idx * size_));
  }

private:
  friend PipeTester;

  uint64_t next(uint64_t current) const {
    ASSERT_IDX_VALIDATION(current);

    uint64_t next_pos = current + 1;
    if (next_pos >= end_) {
      return 0;
    } else {
      return next_pos;
    }
  }

  std::unique_ptr<uint8_t[]> pipe_;
  uint64_t head_[2];
  // Reference head_[0];
  uint64_t* read_idx_  GUARDED_BY(read_mutex_);
  // Reference head_[1];
  uint64_t* write_idx_ GUARDED_BY(write_mutex_);

  const uint64_t begin_;
  const uint64_t end_;

  const uint8_t* begin_addr_;
  const uint8_t* end_addr_;

  // Size of message in pipe
  size_t size_;
  // Number of messages in pipe
  size_t length_;

  std::mutex read_mutex_;
  std::mutex write_mutex_;
};

template<typename T, typename U>
struct BiPipeParam {
  const T* in_message;
  size_t   in_length;

  const T* out_message;
  size_t   out_length;
};

template<typename T, typename U>
class BiPipe;

template<typename T, typename U>
class MinorBiPipe {
public:
  bool write(const T& message) {
    return out_->write(message);
  }

  std::optional<T> read() {
    return in_->read();
  }

  std::optional<T> peek() {
    return in_->peek();
  }

  bool readable() const {
    return !in_->isEmpty();
  }

  bool writable() const {
    return !out_->isFull();
  }
private:
  friend BiPipe<T, U>;

  MinorBiPipe(RingPipe<T>* in, RingPipe<U>* out):
    in_{in}, out_{out} {}

  RingPipe<T>* in_;
  RingPipe<U>* out_;
};


template<typename T, typename U>
class BiPipe {
public:
  BiPipe(BiPipeParam<T,U>& param):
    in_ {*param.in_message, param.in_length},
    out_{*param.out_message, param.out_length} {}

  MinorBiPipe<U, T> CreateMinor() {
    return MinorBiPipe<U, T>(&out_, &in_);
  }

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
