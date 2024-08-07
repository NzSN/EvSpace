#ifndef EVSPACE_EVAL_ASYNC_PIPE_H_
#define EVSPACE_EVAL_ASYNC_PIPE_H_

#include <chrono>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <cstdint>
#include <memory>
#include <stddef.h>
#include <optional>
#include <string.h>
#include <type_traits>

#include "base/assert.h"
#include "base/thread_safety.h"

#include "eval/async/traits.h"

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

template<typename T>
struct RingPipeMeta;

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
      .instance = this,
    };

    return meta;
  }

#define ASSERT_VALIDITY_OF_IDX(IDX) \
  ASSERT(begin_ <= IDX && IDX < end_, "INVALID PIPE IDX")
#define ASSERT_VALIDITY_OF_ADDR(ADDR) \
  ASSERT(begin_addr_ <= ADDR && ADDR < end_addr_, "INVALID PIPE ADDRESS")

  RingPipe(size_t length):
    pipe_       {std::make_unique<uint8_t[]>(length == 0 ? 1 : length)},
    rw_head_    {std::make_unique<uint32_t[]>(2)},
    r_idx_      {&rw_head_[0]},
    w_idx_      {&rw_head_[1]},
    begin_      {0},
    end_        {length == 0 ? 1 : static_cast<uint32_t>(length)},
    begin_addr_ {pipe_.get()},
    end_addr_   {pipe_.get() + (length == 0 ? 1 : length)},
    length_     {length == 0 ? 1 : length} {

    memset(pipe_.get(), 0, length);
    memset(rw_head_.get(), 0, sizeof(uint32_t) * 2);
  }

  RingPipe(const RingPipe&) = delete;
  RingPipe& operator=(RingPipe&) = delete;

  RingPipe(RingPipe&& other):
    pipe_       {std::move(other.pipe_)},
    rw_head_    {std::move(other.rw_head_)},
    r_idx_      {other.r_idx_},
    w_idx_      {other.w_idx_},
    begin_      {other.begin_},
    end_        {
      other.length_ < UINT32_MAX ?
      static_cast<decltype(end_)>(other.length_) :
      UINT32_MAX
    },
    begin_addr_ {other.begin_addr_},
    end_addr_   {other.end_addr_},
    length_     {other.length_} {}

  RingPipe& operator=(RingPipe&& other) {
    this->pipe_ = std::move(other.pipe_);

    this->rw_head_[0] = other.rw_head_[0];
    this->rw_head_[1] = other.rw_head_[1];

    this->r_idx_ = other.r_idx_;
    this->w_idx_ = other.w_idx_;

    this->begin_ = other.begin_;
    this->end_ = other.end_;

    this->begin_addr_ = other.begin_addr_;
    this->end_addr_ = other.end_addr_;

    this->length_ = other.length_;

    return *this;
  }

  size_t length() const {
    return length_;
  }

  uint8_t* pipe() {
    return pipe_.get();
  }

  template<bool blocked = true,
           bool consecutive = false,
           typename = std::enable_if_t<blocked>>
  std::optional<T> read() EXCLUDES(r_mutex_) {
    std::lock_guard<std::mutex> lock{r_mutex_};

    // Precondition Check
    ASSERT_VALIDITY_OF_IDX(*r_idx_);

    if (isEmpty()) {
      if (!blocked) return std::nullopt;
      else {
        waitNonempty();
      }
    }

    uint8_t* slot_to_read = addrOfIdx(*r_idx_);
    ASSERT_VALIDITY_OF_ADDR(slot_to_read);

    uint8_t bytes_to_read = *slot_to_read;
    ASSERT(bytes_to_read > 0,
           "Read zero bytes from non-empty buffer is trivial");

    if (!blocked && UnreadBytesInBuffer() < bytes_to_read) {
      return std::nullopt;
    }

    slot_to_read = nextN(slot_to_read, 1);
    updateIdx<R_IDX>(slot_to_read);

    return readFromBuffer<consecutive>(slot_to_read, bytes_to_read);
  }

  template<bool blocked = true,
           bool consecutive = false,
           typename = std::enable_if_t<blocked>>
  bool write(const T& message) EXCLUDES(w_mutex_) {
    std::lock_guard<std::mutex> lock{w_mutex_};

    ASSERT_VALIDITY_OF_IDX(*w_idx_);

    size_t bytes_to_write = message.ByteSizeLong();
    ASSERT(bytes_to_write < UINT8_MAX);

    if (bytes_to_write <= 0) {
      return true;
    } else if (bytes_to_write > UINT8_MAX) {
      return false;
    }

    if (!blocked && FreeBytesInBuffer() < bytes_to_write) {
      return false;
    } else {
      waitNonfull();
    }

    uint8_t* current = addrOfIdx(*w_idx_);
    // Write Byte indicator
    *current = static_cast<uint8_t>(bytes_to_write);
    current = nextN(current, 1);
    updateIdx<W_IDX>(current);

    // Write message into buffer
    writeIntoBuffer<consecutive>(message, current);
    return true;
  }

  bool isEmpty() const NO_THREAD_SAFETY_ANALYSIS {
    return *r_idx_ == *w_idx_;
  }

  bool isFull() const NO_THREAD_SAFETY_ANALYSIS {
    return length_ == 0 || next(*w_idx_) == *r_idx_;
  }

  // Primaryly called by function
  // defined in JS Realm
  void notify_one() {
    cv_.notify_one();
  }
  void notify_all() {
    cv_.notify_all();
  }

private:
  friend PipeTester;
  friend EVAL::SPACE::WASM_SPACE::CODEGEN::EmPipeMeta<T>;
  friend EVAL
          ::SPACE
          ::NODE_NATIVE_SPACE
          ::CODEGEN
          ::AsyncStructMapping<RingPipe<T>>;

  enum RW_HEAD {
    R_IDX,
    W_IDX,
  };

  uint8_t* addrOfIdx(uint32_t idx) {
    return const_cast<uint8_t*>(begin_addr_ + idx);
  }

  uint32_t IdxOfAddr(uint8_t* addr) {
    ASSERT_VALIDITY_OF_ADDR(addr);
    return addr - begin_addr_;
  }

  template<RW_HEAD head, typename = std::enable_if_t<head == R_IDX>,
           typename = int>
  void updateIdx(uint8_t* addr) REQUIRES(r_mutex_) EXCLUDES(cv_mutex_) {
    std::lock_guard lock(cv_mutex_);
    *r_idx_ = IdxOfAddr(addr);
    cv_.notify_one();
  }

  template<RW_HEAD head, typename = std::enable_if_t<head == W_IDX>>
  void updateIdx(uint8_t* addr) REQUIRES(w_mutex_) EXCLUDES(cv_mutex_){
    std::lock_guard lock(cv_mutex_);
    *w_idx_ = IdxOfAddr(addr);
    cv_.notify_one();
  }

  uint32_t next(uint32_t current) const {
    ASSERT_VALIDITY_OF_IDX(current);

    uint32_t next_pos = current + 1;
    if (next_pos >= end_) {
      return 0;
    } else {
      return next_pos;
    }
  }

  uint8_t* nextN(uint8_t* addr, size_t offset) {
    ASSERT_VALIDITY_OF_ADDR(addr);
    ASSERT(end_addr_ - addr - 1 >= 0);

    if (static_cast<size_t>(end_addr_ - addr - 1) >= offset) {
      return addr + offset;
    } else {
      return begin_addr_ + (offset - (end_addr_ - addr - 1)) - 1;
    }
  }

  void waitNonfull() EXCLUDES(cv_mutex_) {
    std::unique_lock lock(cv_mutex_);
    cv_.wait(lock, [&]{ return !isFull(); });
  }

  void waitNonempty() EXCLUDES(cv_mutex_) {
    std::unique_lock lock(cv_mutex_);
    cv_.wait(lock, [&]{ return !isEmpty(); });
  }

  size_t UnreadBytesInBuffer() const NO_THREAD_SAFETY_ANALYSIS {
    uint32_t r_idx_v = *r_idx_;
    uint32_t w_idx_v = *w_idx_;

    if (r_idx_v > w_idx_v) {
      return length_ - (r_idx_v - w_idx_v);
    } else {
      return w_idx_v - r_idx_v;
    }
  }

  size_t FreeBytesInBuffer() const {
    return length() - UnreadBytesInBuffer() - 1;
  }

  size_t FreeBytesToEnd(uint8_t* addr) const {
    ASSERT_VALIDITY_OF_ADDR(addr);
    return end_addr_ - addr - 1;
  }

  size_t UnreadBytesToEnd(uint8_t* addr) const {
    ASSERT_VALIDITY_OF_ADDR(addr);
    return end_addr_ - addr;
  }

  template<bool consecutive = false,
           typename = std::enable_if_t<!consecutive>>
  bool writeIntoBuffer(const T& message, uint8_t* current)
    REQUIRES(w_mutex_) {

    size_t bytes_to_write = message.ByteSizeLong(),
      remain = bytes_to_write;
    std::unique_ptr<uint8_t[]> msg_buffer =
      std::make_unique<uint8_t[]>(bytes_to_write);
    message.SerializeToArray(msg_buffer.get(), bytes_to_write);

    uint8_t* msg_buffer_current = msg_buffer.get();

    while (remain > 0) {
      size_t n_bytes_writable = std::min(
        FreeBytesInBuffer(), remain);
      if (n_bytes_writable == 0) {
        waitNonfull();
        continue;
      }

      ASSERT(msg_buffer.get() <= msg_buffer_current &&
             msg_buffer_current < msg_buffer.get() + bytes_to_write);

      if (n_bytes_writable <= FreeBytesToEnd(current)) {
        writeConsecutive(&current, &msg_buffer_current, n_bytes_writable);
      } else {
        writeCrossEnd(&current, &msg_buffer_current, n_bytes_writable);
      }

      updateIdx<W_IDX>(current);
      remain -= n_bytes_writable;
    }

    return true;
  }

  void writeConsecutive(uint8_t** current,
                        uint8_t** msg_buffer,
                        size_t writable) REQUIRES(w_mutex_) {
    uint8_t* ptr = *current, *msg_ptr = *msg_buffer;

    memcpy(ptr, msg_ptr, writable);
    *current = nextN(*current, writable);
    *msg_buffer = msg_ptr + writable;
  }

  void writeCrossEnd(uint8_t** current,
                     uint8_t** msg_buffer,
                     size_t writable) REQUIRES(w_mutex_) {
    uint8_t* ptr = *current, *msg_ptr = *msg_buffer;

    size_t bytes_to_write_1 = end_addr_ - ptr,
      bytes_to_write_2 = writable - bytes_to_write_1;
    memcpy(ptr, msg_ptr, bytes_to_write_1);
    memcpy(begin_addr_, msg_ptr + bytes_to_write_1,
           bytes_to_write_2);
    *current = nextN(ptr, writable);
    *msg_buffer = msg_ptr + writable;
  }

  void readCrossEnd(uint8_t** current,
                    uint8_t** msg_buffer_pos,
                    size_t readable) {
    uint8_t* current_ptr = *current,
      *msg_buffer_pos_ptr = *msg_buffer_pos;

    size_t read_bytes_1 = end_addr_ - current_ptr,
      read_bytes_2 = readable - read_bytes_1;

    memcpy(msg_buffer_pos_ptr, current_ptr, read_bytes_1);
    memcpy(msg_buffer_pos_ptr + read_bytes_1, begin_addr_, read_bytes_2);

    *current = nextN(*current, readable);
    *msg_buffer_pos = msg_buffer_pos_ptr + readable;
  }

  template<bool consecutive = false,
           typename = std::enable_if_t<consecutive>,
           typename = int>
  bool writeIntoBuffer(const T& message, uint8_t* current)
    REQUIRES(w_mutex_) {
    return true;
  }

  template<bool consecutive = false,
           typename = std::enable_if_t<!consecutive>>
  std::optional<T> readFromBuffer(uint8_t* current, size_t bytes_to_read)
    REQUIRES(r_mutex_) {

    std::unique_ptr<uint8_t[]> msg_buffer =
      std::make_unique<uint8_t[]>(bytes_to_read);
    uint8_t* msg_buffer_pos = msg_buffer.get();

    size_t remain = bytes_to_read;
    while (remain > 0) {
      size_t bytes_in_buffer = std::min(
        UnreadBytesInBuffer(), remain);
      if (bytes_in_buffer == 0) {
        waitNonempty();
        continue;
      }

      if (UnreadBytesToEnd(current) > bytes_in_buffer) {
        memcpy(msg_buffer_pos, current, bytes_in_buffer);
        current = nextN(current, bytes_in_buffer);
        msg_buffer_pos += bytes_in_buffer;
      } else {
        readCrossEnd(&current, &msg_buffer_pos, bytes_in_buffer);
      }

      updateIdx<R_IDX>(current);
      remain -= bytes_in_buffer;
    }

    T message;
    if (!message.ParseFromArray(msg_buffer.get(), bytes_to_read)) {
      return std::nullopt;
    } else {
      return message;
    }
  }

  std::unique_ptr<uint8_t[]> pipe_;
  std::unique_ptr<uint32_t[]> rw_head_;
  // Reference rw_head_[0];
  uint32_t* r_idx_  GUARDED_BY(r_mutex_);
  // Reference rw_head_[1];
  uint32_t* w_idx_ GUARDED_BY(w_mutex_);

  uint32_t begin_;
  uint32_t end_;

  uint8_t* begin_addr_;
  uint8_t* end_addr_;

  // Length of buffer
  size_t length_;

  std::mutex r_mutex_;
  std::mutex w_mutex_;

  std::mutex cv_mutex_;
  std::condition_variable cv_;

  #undef ASSERT_VALIDITY_OF_IDX
  #undef ASSERT_VALIDITY_OF_ADDR
};

template<typename T>
struct RingPipeMeta {
  using type = T;

  uint8_t* pipe;
  uint32_t* rw_head;
  size_t length;

  RingPipe<T>* instance;
};


template<typename T, typename U>
struct BiPipeParam {
  size_t   in_length;
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
    in_ {param.in_length},
    out_{param.out_length} {}
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
