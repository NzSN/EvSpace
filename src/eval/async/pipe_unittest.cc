#include <gtest/gtest.h>
#include <rapidcheck/gtest.h>

#include "pipe.h"

namespace EVSPACE {
namespace ASYNC {

struct Message {
  Message(): data_{} {}
  Message(unsigned int data): data_{data} {}

  bool SerializeToArray(void* data, size_t size) const {
    if (size < sizeof(unsigned int)) {
      return false;
    }
    unsigned int* data_uint = reinterpret_cast<unsigned int*>(data);
    *data_uint = data_;
    return true;
  }

  bool ParseFromArray(const void* data, size_t size) {
    const unsigned int* data_uint = reinterpret_cast<const unsigned int*>(data);
    data_ = *data_uint;
    return true;
  }

  size_t ByteSizeLong() const {
    return sizeof(unsigned int);
  }

  unsigned int getData() {
    return data_;
  }

private:
  unsigned int data_;
};

struct PipeTester : public ::testing::Test {
  PipeTester(): pipe{Message{1}, length} {}

  static constexpr size_t length = 1000;
  MessagePipe<Message> pipe;
};


RC_GTEST_FIXTURE_PROP(PipeTester, Spec, ()) {
  RC_ASSERT(pipe.isEmpty());

  for (size_t i = 0; i < length; ++i) {
    Message msg{static_cast<unsigned int>(i)};
    pipe.write(msg);
  }
  RC_ASSERT(pipe.isFull());

  for (size_t i = 0; i < length; ++i) {
    auto msg = pipe.read();
    if (msg.has_value()) {
      RC_ASSERT(msg.value().getData() == i);
    }
  }

  RC_ASSERT(pipe.isEmpty());
}

struct BiPipeTester : public ::testing::Test {

  void SetUp() override {
    Message msg_in{1}, msg_out{1};
    BiPipeParam<Message,Message> param {
      .in_message  = &msg_in,
      .in_length   = in_length,
      .out_message = &msg_out,
      .out_length  = out_length
    };
    pipe = std::make_unique<BiPipe<Message,Message>>(param);
  }

  static constexpr size_t in_length = 10, out_length = 10;
  std::unique_ptr<BiPipe<Message,Message>> pipe;
};

RC_GTEST_FIXTURE_PROP(BiPipeTester, Spec, ()) {

}

} // ASYNC
} // EVSPACE
