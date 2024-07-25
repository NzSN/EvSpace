#include "eval/async/messages/counter.pb.h"


int main() {
  Counter counter;

  counter.set_counter(10);
  std::cout << "Counter: " << counter.counter() << std::endl;
  std::cout << "ByteSize:" << counter.ByteSizeLong() << std::endl;

  uint8_t* bytes = new uint8_t(counter.ByteSizeLong());
  counter.SerializeToArray(bytes, counter.ByteSizeLong());


  Counter counter_copy;
  counter_copy.ParseFromArray(bytes, 2);

  std::cout << "Counter_copy:" << counter_copy.counter() << std::endl;
}
