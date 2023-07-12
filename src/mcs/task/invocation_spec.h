//
// Created by lianyu on 2023/1/9.
//

#pragma once

#include <mcs/api/mcs_runtime.h>
#include <mcs/api/common_types.h>
#include <msgpack.hpp>

// #include "mcs/common/id.h"core
#include "mcs/core/buffer.h"

namespace mcs {
  namespace internal {

    class InvocationSpec {
    public:
      TaskType task_type;
      std::string name;
      // ActorID actor_id;
      // int actor_counter;
      RemoteFunctionHolder remote_function_holder;
      std::vector<std::shared_ptr<LocalMemoryBuffer>> args;
    };
  }  // namespace internal
}  // namespace mcs
