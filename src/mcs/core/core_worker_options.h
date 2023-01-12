//
// Created by lianyu on 2023/1/5.
//

#pragma once
#include <functional>
#include "mcs/common/status.h"


namespace mcs {
  namespace core {

    struct CoreWorkerOptions {
      using TaskExecutionCallback = std::function<Status(
              const std::string task_name
      )>;

    };
  } //namespace core
} //namespace mcs
