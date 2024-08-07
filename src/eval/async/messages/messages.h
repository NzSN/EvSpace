#ifndef EVSPACE_EVAL_ASYNC_MESSAGES_MESSAGES_H_
#define EVSPACE_EVAL_ASYNC_MESSAGES_MESSAGES_H_

#include "eval/async/messages/counter.pb.h"

namespace EVSPACE {
namespace ASYNC {
namespace MESSAGE {

#define DECLARED_MESSAGES(V) \
  V(Counter)


template<typename T>
struct MessageType;

#define DECLARE_MESSAGE_TYPE_STR(MSG)               \
  template<>                                        \
  struct MessageType<MSG> {                         \
    static constexpr const char* msg_type = #MSG;   \
  };

DECLARED_MESSAGES(DECLARE_MESSAGE_TYPE_STR)
#undef DECLARE_MESSAGE_TYPE_STR



} // MESSAGE
} // ASYNC
} // EVSPACE

#endif /* EVSPACE_EVAL_ASYNC_MESSAGES_MESSAGES_H_ */
