#include <chrono>
#include <cstdlib>
#include <gtest/gtest.h>
#include <rapidcheck/gtest.h>
#include <thread>
#include <vector>

#include "pipe.h"
#include "base/utility.h"

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

  bool operator==(const Message& other) const {
    return this->data_ == other.data_;
  }

private:
  unsigned int data_;
};

struct PipeTester : public ::testing::Test {
  RingPipe<Message> CreateTestPipe(size_t length) {
    return RingPipe<Message>{Message{1}, length};
  }
};

RC_GTEST_FIXTURE_PROP(PipeTester, Spec, ()) {
  size_t length = *rc::gen::inRange(0, 1000);

  RingPipe<Message> pipe = CreateTestPipe(length);

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

constexpr size_t RANDOM_MAX = 100;
using RANDOM = BASE::UTILITY::Random<1,RANDOM_MAX>;
using RANDOM_DELAY = BASE::UTILITY::Random<1,5>;
void ReadPipe(RingPipe<Message>* pipe, size_t times) {
  size_t last_v = 0;
  while (times > 0) {
    bool wait = RANDOM{}() < (RANDOM_MAX/10);
    if (wait) {
      std::this_thread::sleep_for(
        std::chrono::milliseconds(RANDOM_DELAY{}()));
    }

    auto msg = pipe->read();
    if (!msg.has_value()) {
      // Wait a moment
      std::this_thread::sleep_for(
        std::chrono::milliseconds(3));
      continue;
    } else {
      ASSERT(msg.value().getData() == last_v);
      ++last_v;
    }

    --times;
  }
}
void WritePipe(RingPipe<Message>* pipe, size_t times) {
  unsigned int v = 0;
  while (times > 0) {
    bool wait = RANDOM{}() < (RANDOM_MAX/10);
    if (wait) {
      std::this_thread::sleep_for(
        std::chrono::milliseconds(RANDOM_DELAY{}()));
    }

    Message msg{v++};
    while (!pipe->write(msg)) {
      std::this_thread::sleep_for(
        std::chrono::milliseconds(3));
    }

    --times;
  }
}

RC_GTEST_FIXTURE_PROP(PipeTester, ParallelReadAndWrite, ()) {
  size_t length = *rc::gen::inRange(2, 1000);
  size_t times  = *rc::gen::inRange(0, 100);
  RingPipe<Message> pipe = CreateTestPipe(length);
  RC_ASSERT(pipe.isEmpty());

  std::thread t_w{WritePipe, &pipe, times};
  std::thread t_r{ReadPipe,  &pipe, times};

  t_w.join();
  t_r.join();

  RC_ASSERT(pipe.isEmpty());
}

void SendMessageToPipe(RingPipe<Message>* pipe, Message msg, size_t times) {
  while (times > 0) {
    bool wait = RANDOM{}() < (RANDOM_MAX/10);
    if (wait) {
      std::this_thread::sleep_for(
        std::chrono::milliseconds(RANDOM_DELAY{}()));
    }

    while (!pipe->write(msg)) {
      std::this_thread::sleep_for(
        std::chrono::milliseconds(3));
    }

    --times;
  }
}

void ReceiveMessageFromPipe(RingPipe<Message>* pipe, Message msg, size_t times) {
  while (times > 0) {
    bool wait = RANDOM{}() < (RANDOM_MAX/10);
    if (wait) {
      std::this_thread::sleep_for(
        std::chrono::milliseconds(RANDOM_DELAY{}()));
    }

    auto msgFromPipe = pipe->peek();
    if (!msgFromPipe.has_value()) {
      std::this_thread::sleep_for(
        std::chrono::milliseconds(3));
      continue;
    } else {
      // To check that wheter the msg is what we want
      if (msgFromPipe == msg) {
        msgFromPipe = pipe->read();
      } else {
        continue;
      }
    }

    --times;
  }
}

RC_GTEST_FIXTURE_PROP(PipeTester, MultiReadAndMultiWrite, ()) {
  size_t numOfThreadPairs = *rc::gen::inRange(1, 10);
  size_t length = *rc::gen::inRange(2, 1000);
  size_t times  = *rc::gen::inRange(0, 100);

  RingPipe<Message> pipe = CreateTestPipe(length);
  RC_ASSERT(pipe.isEmpty());

  std::vector<std::thread> r_threads;
  std::vector<std::thread> w_threads;

  for (size_t i = 0; i < numOfThreadPairs; ++i) {
    r_threads.emplace_back(ReceiveMessageFromPipe,
                           &pipe,
                           Message{static_cast<unsigned int>(i)},
                           times);
    w_threads.emplace_back(SendMessageToPipe,
                           &pipe,
                           Message{static_cast<unsigned int>(i)},
                           times);
  }

  for (auto& t: r_threads) {
    t.join();
  }
  for (auto& t: w_threads) {
    t.join();
  }

  RC_ASSERT(pipe.isEmpty());
}

struct BiPipeTester : public ::testing::Test {
  BiPipe<Message,Message> CreateBiPipe(
    BiPipeParam<Message,Message>& param) {

    return BiPipe<Message,Message>{param};
  }
};

RC_GTEST_FIXTURE_PROP(BiPipeTester, Spec, (int a)) {
  Message msg_in{1}, msg_out{2};
  BiPipeParam<Message, Message> param = {
    .in_message  = &msg_in,
    .in_length   = static_cast<size_t>(*rc::gen::inRange(2, 1000)),
    .out_message = &msg_out,
    .out_length  = static_cast<size_t>(*rc::gen::inRange(2, 1000))
  };


}

} // ASYNC
} // EVSPACE
