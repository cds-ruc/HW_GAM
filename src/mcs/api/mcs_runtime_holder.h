//
// Created by lianyu on 2023/1/6.
//

#pragma once

#include "mcs_runtime.h"

namespace mcs {
  namespace internal {

    struct McsRuntimeHolder {
      static McsRuntimeHolder &Instance() {
        static McsRuntimeHolder instance;
        return instance;
      }

      void Init(std::shared_ptr<McsRuntime> runtime) { runtime_ = runtime; }

      std::shared_ptr<McsRuntime> Runtime() { return runtime_; }

    private:
      McsRuntimeHolder() = default;
      ~McsRuntimeHolder() = default;
      McsRuntimeHolder(McsRuntimeHolder const &) = delete;
      McsRuntimeHolder(McsRuntimeHolder &&) = delete;
      McsRuntimeHolder &operator=(McsRuntimeHolder const &) = delete;
      McsRuntimeHolder &operator=(McsRuntimeHolder &&) = delete;

      std::shared_ptr<McsRuntime> runtime_;
    };

    inline static std::shared_ptr<McsRuntime> GetMcsRuntime() {
      return McsRuntimeHolder::Instance().Runtime();
    }

  }  // namespace internal
}  // namespace mcs
