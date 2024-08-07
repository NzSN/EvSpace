#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <exception>
#include <gtest/gtest.h>
#include <mutex>
#include <optional>
#include <rapidcheck/gtest.h>
#include <thread>
#include <limits.h>

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
    return RingPipe<Message>{length};
  }
};

void Writer(RingPipe<Message>* pipe, size_t times) {
  for (size_t i = 0; i < times; ++i) {
    Message msg{static_cast<unsigned int>(i)};
    pipe->write(msg);
  }
}

void Reader(RingPipe<Message>* pipe, size_t times) {
  for (size_t i = 0; i < times; ++i) {
    auto msg = pipe->read();
    RC_ASSERT(msg.value().getData() == i);
  }
}

RC_GTEST_FIXTURE_PROP(PipeTester, Simple, ()) {
  size_t length = *rc::gen::inRange(10, 100);
  size_t times = *rc::gen::inRange(1, 1000);

  RingPipe<Message> pipe = CreateTestPipe(length);
  RC_ASSERT(pipe.isEmpty());

  std::thread t_w{Writer, &pipe, times};
  std::thread t_r{Reader, &pipe, times};

  t_w.join();
  t_r.join();

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

struct BiPipeTester : public ::testing::Test {
  BiPipe<Message,Message> CreateBiPipe(
    BiPipeParam<Message,Message>& param) {

    return BiPipe<Message,Message>{param};
  }
};

bool processed = false;
template<unsigned int num, unsigned int upperbound>
void NumProducer(
  MinorBiPipe<Message,Message>* pipe,
  unsigned int* total,
  std::mutex* mutex,
  std::condition_variable* cv,
  std::mutex* p_mutex) {

  while (true) {
    {
      std::lock_guard lock{*p_mutex};

      pipe->write(Message{num});
      *total += num;

      std::optional<Message> msg = pipe->read();
      if (msg.has_value()) {
        if (msg.value().getData() >= upperbound) {
          pipe->write(Message{0});
          return;
        }
      }

      {
        std::unique_lock lock{*mutex};
        cv->wait(lock, [&]{ return processed; });
        processed = false;
      }
    }
  }
}

void Adder(
  BiPipe<Message,Message>* pipe,
  unsigned int* sum,
  std::mutex* mutex,
  std::condition_variable* cv,
  size_t num_of_producer) {

  while (true) {
      auto msg = pipe->read();
      if (msg.has_value()) {
        if (msg.value().getData() == 0) {
          if (--num_of_producer == 0) {
            return;
          } else {
            continue;
          }
        }
        if (UINT_MAX - *sum >= msg.value().getData()) {
          *sum += msg.value().getData();
        }
      }

      if (pipe->writable()) {
        pipe->write(Message{*sum});
      }

      {
        std::lock_guard lock{*mutex};
        processed = true;
        cv->notify_one();
      }
  }
}

RC_GTEST_FIXTURE_PROP(BiPipeTester, Spec, ()) {
  BiPipeParam<Message, Message> param = {
    .in_length   = static_cast<size_t>(*rc::gen::inRange(2, 1000)),
    .out_length  = static_cast<size_t>(*rc::gen::inRange(2, 1000))
  };

  std::mutex p_mutex;
  std::mutex mutex;
  std::condition_variable cv;

  auto pipe = CreateBiPipe(param);
  auto minor = pipe.CreateMinor();

  unsigned int sum = 0;
  size_t numOfProducer = *rc::gen::inRange(1, 10);

  std::thread t_adder{Adder, &pipe, &sum, &mutex, &cv, numOfProducer};

  std::vector<std::thread> producers;
  std::vector<unsigned int> totals(numOfProducer);

  for (size_t i = 0; i < numOfProducer; ++i) {
    producers.emplace_back(
      NumProducer<1, 10>, &minor, &totals[i], &mutex, &cv, &p_mutex);
  }

  t_adder.join();
  for (auto& producer: producers) {
    producer.join();
  }

  unsigned int total = 0;
  for (auto num: totals) {
    total += num;
  }

  RC_ASSERT(sum == total);
}

} // ASYNC
} // EVSPACE
