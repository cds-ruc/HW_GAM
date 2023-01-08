//
// Created by hrh on 1/4/23.
//

#include "api.h"
#include "mcs/core/config_internal.h"
//#include "mcs/core_worker/core_worker.h"
#include "mcs/runtime/abstract_mcs_runtime.h"

namespace mcs {

  static bool is_init_ = false;

  void Init(McsConfig &config, int argc, char **argv) {
    if (!IsInitialized()) {
      internal::ConfigInternal::Instance().Init(config, argc, argv);
      auto runtime = internal::AbstractMcsRuntime::DoInit();
      is_init_ = true;
    }
  }

  void Init(mcs::McsConfig &config) { Init(config, 0, nullptr); }

  void Init() {
    McsConfig config;
    Init(config, 0, nullptr);
  }

  bool IsInitialized() { return is_init_; }

  void Shutdown() {
    internal::AbstractMcsRuntime::DoShutdown();
    is_init_ = false;
  }

//  void RunTaskExecutionLoop() { ::ray::core::CoreWorkerProcess::RunTaskExecutionLoop(); }

} // namespace mcs