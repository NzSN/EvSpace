#ifndef EVSPACE_EVAL_ASYNC_PIPE_H_
#define EVSPACE_EVAL_ASYNC_PIPE_H_

#include "stddef.h"

#define ASYNC PIPE

extern "C" {
  struct PIPE {
    void* data;
    size_t size;
  };
}

#endif /* EVSPACE_EVAL_ASYNC_PIPE_H_ */
