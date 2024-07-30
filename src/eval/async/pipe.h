#ifndef EVSPACE_EVAL_ASYNC_PIPE_H_
#define EVSPACE_EVAL_ASYNC_PIPE_H_

#include <cstring>
#include <mutex>
#include <cstdint>
#include <memory>
#include <stddef.h>
#include <optional>

#include "base/assert.h"
#include "base/thread_safety.h"
#include "base/traits.h"
#include "base/utility.h"

#define ASYNCHRONOUS(T) EVSPACE::ASYNC::SymPipeMeta<T>

namespace EVSPACE {
namespace EVAL {
namespace SPACE {
namespace WASM_SPACE {
namespace CODEGEN {

template<typename T>
struct EmPipeMeta;

template<typename T, typename U>
struct EmBiPipeMeta;


} // CODEGEN
} // WASM_SPACE
} // SPACE
} // EVAL
} // EVSPACE

namespace EVSPACE {
namespace EVAL {
namespace SPACE {
namespace NODE_NATIVE_SPACE {
namespace CODEGEN {

template<typename T>
struct AsyncStructMapping;

} // CODEGEN
} // NODE_NATIVE_SPACE
} // SPACE
} // EVAL
} // EVSPACE

namespace EVSPACE {
namespace ASYNC {

struct PipeTester;

DEFINE_TRAIT_HAS_METHOD(ByteSizeLong);
DEFINE_TRAIT_HAS_METHOD(SerializeToArray);
DEFINE_TRAIT_HAS_METHOD(ParseFromArray);

template<typename T>
struct RingPipeMeta {
  using type = T;

  uint8_t* pipe;
  uint32_t* rw_head;
  size_t length;
  size_t msgSize;
};


template<typename T,
         typename = std::enable_if<
           has_method_ByteSizeLong<T,size_t()>::value &&
           has_method_SerializeToArray<T, bool(void*,size_t)>::value &&
           has_method_ParseFromArray<T, bool(const void*, size_t)>::value>>
class RingPipe {
public:
  RingPipeMeta<T> generateMeta() {
    RingPipeMeta<T> meta = {
      .pipe = pipe(),
      .rw_head = &rw_head_[0],
      .length = length(),
      .msgSize = msgSize()
    };

    return meta;
  }

#define ASSERT_VALIDITY_OF_IDX(IDX) \
  ASSERT(begin_ <= IDX && IDX < end_, "INVALID PIPE IDX")
#define ASSERT_VALIDITY_OF_ADDR(ADDR) \
  ASSERT(begin_addr_ <= ADDR && ADDR < end_addr_, "INVALID PIPE ADDRESS")

  RingPipe(const T& message, size_t length):
    pipe_       {std::make_unique<uint8_t[]>(message.ByteSizeLong() * length)},
    rw_head_    {std::make_unique<uint32_t[]>(2)},
    read_idx_   {&rw_head_[0]},
    write_idx_  {&rw_head_[1]},
    begin_      {0},
    end_        {static_cast<uint32_t>(length)},
    begin_addr_ {pipe_.get()},
    end_addr_   {pipe_.get() + message.ByteSizeLong() * length},
    msgSize_    {message.ByteSizeLong()},
    length_     {length} {

    ASSERT(IS_ALIGNED(pipe_.get(), msgSize_));
    memset(pipe_.get(), 0, message.ByteSizeLong() * length);
    memset(rw_head_.get(), 0, sizeof(uint32_t) * 2);
  }

  RingPipe(const RingPipe&) = delete;
  RingPipe& operator=(RingPipe&) = delete;

  RingPipe(RingPipe&& other):
    pipe_       {std::move(other.pipe_)},
    rw_head_    {std::move(other.rw_head_)},
    read_idx_   {other.read_idx_},
    write_idx_  {other.write_idx_},
    begin_      {other.begin_},
    end_        {other.length_},
    begin_addr_ {other.begin_addr_},
    end_addr_   {other.end_addr_},
    msgSize_    {other.msgSize_},
    length_     {other.length_}
                {}

  RingPipe& operator=(RingPipe&& other) {
    this->pipe_ = std::move(other.pipe_);

    this->rw_head_[0] = other.rw_head_[0];
    this->rw_head_[1] = other.rw_head_[1];

    this->read_idx_ = other.read_idx_;
    this->write_idx_ = other.write_idx_;

    this->begin_ = other.begin_;
    this->end_ = other.end_;

    this->begin_addr_ = other.begin_addr_;
    this->end_addr_ = other.end_addr_;

    this->msgSize_ = other.msgSize_;
    this->length_ = other.length_;

    return *this;
  }

  size_t msgSize() const {
    return msgSize_;
  }

  size_t length() const {
    return length_;
  }

  uint8_t* pipe() {
    return pipe_.get();
  }

  std::optional<T> read() EXCLUDES(read_mutex_) {
    std::lock_guard<std::mutex> lock{read_mutex_};

    auto message = peekWithoutSync();
    if (message.has_value()) {
      *read_idx_ = next(*read_idx_);
    }

    return message;
  }

  std::optional<T> peek() EXCLUDES(read_mutex_) {
    std::lock_guard<std::mutex> lock{read_mutex_};
    return peekWithoutSync();
  }

  bool write(const T& message) EXCLUDES(write_mutex_) {
    std::lock_guard<std::mutex> lock{write_mutex_};

    if (isFull()) {
      return false;
    }

    ASSERT_VALIDITY_OF_IDX(*write_idx_);

    uint8_t* slot_to_write = addrOfIdx(*write_idx_);
    ASSERT_VALIDITY_OF_ADDR(slot_to_write);

    message.SerializeToArray(slot_to_write, msgSize_);
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
    return const_cast<uint8_t*>(begin_addr_ + (idx * msgSize_));
  }

private:
  friend PipeTester;
  friend EVAL::SPACE::WASM_SPACE::CODEGEN::EmPipeMeta<T>;
  friend EVAL
          ::SPACE
          ::NODE_NATIVE_SPACE
          ::CODEGEN
          ::AsyncStructMapping<RingPipe<T>>;

  uint32_t next(uint32_t current) const {
    ASSERT_VALIDITY_OF_IDX(current);

    uint32_t next_pos = current + 1;
    if (next_pos >= end_) {
      return 0;
    } else {
      return next_pos;
    }
  }

  std::optional<T> peekWithoutSync() NO_THREAD_SAFETY_ANALYSIS {
    if (isEmpty()) {
      return std::nullopt;
    }

    ASSERT_VALIDITY_OF_IDX(*read_idx_);

    uint8_t* slot_to_peek = addrOfIdx(*read_idx_);
    ASSERT_VALIDITY_OF_ADDR(slot_to_peek);

    T message;
    message.ParseFromArray(slot_to_peek, msgSize_);

    return message;
  }

  std::unique_ptr<uint8_t[]> pipe_;
  std::unique_ptr<uint32_t[]> rw_head_;
  // Reference rw_head_[0];
  uint32_t* read_idx_  GUARDED_BY(read_mutex_);
  // Reference rw_head_[1];
  uint32_t* write_idx_ GUARDED_BY(write_mutex_);

  uint32_t begin_;
  uint32_t end_;

  uint8_t* begin_addr_;
  uint8_t* end_addr_;

  // Size of message in pipe
  size_t msgSize_;
  // Number of messages in pipe
  size_t length_;

  std::mutex read_mutex_;
  std::mutex write_mutex_;

  #undef ASSERT_VALIDITY_OF_IDX
  #undef ASSERT_VALIDITY_OF_ADDR
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
  friend EVAL
          ::SPACE
          ::NODE_NATIVE_SPACE
          ::CODEGEN
          ::AsyncStructMapping<RingPipe<T>>;


  MinorBiPipe(RingPipe<T>* in, RingPipe<U>* out):
    in_{in}, out_{out} {}

  RingPipe<T>* in_;
  RingPipe<U>* out_;
};

template<typename T, typename U>
struct BiPipeMeta {
  RingPipeMeta<T> in_meta;
  RingPipeMeta<U> out_meta;
};


template<typename T, typename U>
class BiPipe {
public:
  BiPipeMeta<T,U> generateMeta() {
    BiPipeMeta<T,U> meta = {
      .in_meta = in_.generateMeta(),
      .out_meta = out_.generateMeta()
    };

    return meta;
  }

  BiPipe(BiPipeParam<T,U>& param):
    in_ {*param.in_message, param.in_length},
    out_{*param.out_message, param.out_length} {}
  BiPipe(BiPipe<T,U>&& other):
    in_{std::move(other.in_)},
    out_{std::move(other.out_)} {}


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
  friend EVAL::SPACE::WASM_SPACE::CODEGEN::EmBiPipeMeta<T, U>;
  friend EVAL
          ::SPACE
          ::NODE_NATIVE_SPACE
          ::CODEGEN
          ::AsyncStructMapping<RingPipe<T>>;


  RingPipe<T> in_;
  RingPipe<U> out_;
};
template<typename T>
using SymPipeMeta = BiPipeMeta<T,T>;

} // ASYNC
} // EVSPACE

#endif /* EVSPACE_EVAL_ASYNC_PIPE_H_ */
