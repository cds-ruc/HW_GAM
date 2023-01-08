//
// Created by lianyu on 2023/1/5.
//

#pragma once

#include <vector>

#include "mcs/core/config_internal.h"
#include "mcs/core/core_worker.h"
#include "mcs/util/util.h"

namespace mcs {
  namespace internal {

    using mcs::core::CoreWorkerOptions;

    class ProcessHelper {
    public:
      void McsStart(CoreWorkerOptions::TaskExecutionCallback callback);
      void McsStop();
      void StartMcsNode(const int port,
                        const std::vector<std::string> &head_args = {});
      void StopMcsNode();

      static ProcessHelper &GetInstance() {
        static ProcessHelper processHelper;
        return processHelper;
      }

      ProcessHelper(ProcessHelper const &) = delete;
      void operator=(ProcessHelper const &) = delete;

    private:
      ProcessHelper(){};
    };
  } //namespace internal
} //namespace mcs
