//
// Created by lianyu on 2023/1/4.
//
#include <charconv>

#include "config_internal.h"

namespace mcs {
  namespace internal {
    void ConfigInternal::Init(McsConfig &config, int argc, char **argv) {
      if (!config.address.empty()) {
        SetBootstrapAddress(config.address);
      }
      if (argc != 0 && argv != nullptr) {
//        if (worker_type == WorkerType::DRIVER && run_mode == RunMode::CLUSTER) {
//        }
//        if (worker_type == WorkerType::DRIVER) {
//        }
      };
    }

    void ConfigInternal::SetBootstrapAddress(std::string_view address) {
      auto pos = address.find(':');
      bootstrap_ip = address.substr(0, pos);
      auto ret = std::from_chars(
              address.data() + pos + 1, address.data() + address.size(), bootstrap_port);
    }

  } // namespace internal
} // namespace mcs