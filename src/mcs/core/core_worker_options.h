//
// Created by lianyu on 2023/1/5.
//

#pragma once
#include <functional>
#include "mcs/common/status.h"


namespace mcs {
  namespace core {
    enum WorkerType {
      WORKER = 0,
      DRIVER = 1
    };

    struct CoreWorkerOptions {
      using TaskExecutionCallback = std::function<Status(
              const std::string task_name
      )>;

      /// Type of this worker (i.e., DRIVER or WORKER).
      WorkerType worker_type;
    };
  } //namespace core
} //namespace mcs
