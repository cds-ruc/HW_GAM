//
// Created by lianyu on 2023/1/6.
//

#include "local_mode_mcs_runtime.h"

#include "mcs/api.h"

// #include <mcs/runtime/object/local_mode_object_store.h>
// #include <mcs/runtime/object/object_store.h>
#include <mcs/task/local_mode_task_submitter.h>

namespace mcs {
  namespace internal {

    LocalModeMcsRuntime::LocalModeMcsRuntime() {
      // object_store_ = std::unique_ptr<ObjectStore>(new LocalModeObjectStore(*this));
      task_submitter_ = std::unique_ptr<TaskSubmitter>(new LocalModeTaskSubmitter(*this));
    }

//    ActorID LocalModeMcsRuntime::GetNextActorID() {
//      const auto next_task_index = worker_.GetNextTaskIndex();
//      const ActorID actor_id =
//              ActorID::Of(worker_.GetCurrentJobID(), worker_.GetCurrentTaskID(), next_task_index);
//      return actor_id;
//    }

    // const WorkerContext &LocalModeMcsRuntime::GetWorkerContext() { return worker_; }

    // std::string LocalModeMcsRuntime::Put(std::shared_ptr<msgpack::sbuffer> data) {
    //   ObjectID object_id =
    //           ObjectID::FromIndex(worker_.GetCurrentTaskID(), worker_.GetNextPutIndex());
    //   AbstractMcsRuntime::Put(data, &object_id);
    //   return object_id.Binary();
    // }

  }  // namespace internal
}  // namespace mcs
