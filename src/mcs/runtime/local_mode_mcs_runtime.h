//
// Created by lianyu on 2023/1/6.
//

#pragma once

#include <unordered_map>

#include "abstract_mcs_runtime.h"

namespace mcs {
  namespace internal {

    class LocalModeMcsRuntime : public AbstractMcsRuntime {
    public:
      LocalModeMcsRuntime();
//      ActorID GetNextActorID();
      // std::string Put(std::shared_ptr<msgpack::sbuffer> data);
      // const WorkerContext &GetWorkerContext();
      bool IsLocalMode() { return true; }

    private:
      // WorkerContext worker_;
    };

  }  // namespace internal
}  // namespace mcs