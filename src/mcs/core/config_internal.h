//
// Created by lianyu on 2023/1/4.
//

#pragma once
#include <memory>
#include <string>
#include <vector>
#include <string_view>

#include "mcs/mcs_config.h"
#include "core_worker.h"

namespace mcs {
  namespace internal {
    using mcs::core::WorkerType;

    enum class RunMode { SINGLE_PROCESS, CLUSTER };

    class ConfigInternal {
    public:
      WorkerType worker_type = WorkerType::DRIVER;

      RunMode run_mode = RunMode::SINGLE_PROCESS;

      std::string bootstrap_ip;

      int bootstrap_port = 6379;

      int node_manager_port = 0;

      std::vector<std::string> code_search_path;

      std::string node_ip_address = "";

      std::vector<std::string> head_args = {};

      static ConfigInternal &Instance() {
        static ConfigInternal config;
        return config;
      };

      void Init(McsConfig &config, int argc, char **argv);

      void SetBootstrapAddress(std::string_view address);

      ConfigInternal(ConfigInternal const &) = delete;

      void operator=(ConfigInternal const &) = delete;

    private:
      ConfigInternal(){};
    };

  }  // namespace internal
}  // namespace mcs