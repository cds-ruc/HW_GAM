//
// Created by lianyu on 2023/1/9.
//

#pragma once

#include <mcs/api/mcs_runtime.h>

#include <msgpack.hpp>

#include "mcs/common/id.h"
#include "mcs/runtime/task/task_util.h"

namespace mcs {
  namespace internal {

    class InvocationSpec {
    public:
      TaskType task_type;
      std::string name;
      ActorID actor_id;
      int actor_counter;
      RemoteFunctionHolder remote_function_holder;
      std::vector<std::unique_ptr<::mcs::TaskArg>> args;
    };
  }  // namespace internal
}  // namespace mcs
