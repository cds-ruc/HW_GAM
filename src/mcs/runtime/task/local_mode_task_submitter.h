//
// Created by lianyu on 2023/1/9.
//

#pragma once

#include <boost/asio/thread_pool.hpp>
#include <memory>
#include <queue>

#include "mcs/runtime/local_mode_mcs_runtime.h"
#include "absl/synchronization/mutex.h"
#include "invocation_spec.h"
#include "task_executor.h"
#include "task_submitter.h"

namespace mcs {
  namespace internal {

    class LocalModeTaskSubmitter : public TaskSubmitter {
    public:
      LocalModeTaskSubmitter(LocalModeMcsRuntime &local_mode_mcs_tuntime);

      ObjectID SubmitTask(InvocationSpec &invocation, const CallOptions &call_options);

      ActorID CreateActor(InvocationSpec &invocation,
                          const ActorCreationOptions &create_options);

      ObjectID SubmitActorTask(InvocationSpec &invocation, const CallOptions &call_options);

      ActorID GetActor(const std::string &actor_name, const std::string &mcs_namespace) const;

      mcs::PlacementGroup CreatePlacementGroup(
              const mcs::PlacementGroupCreationOptions &create_options);
      void RemovePlacementGroup(const std::string &group_id);
      std::vector<PlacementGroup> GetAllPlacementGroups();
      PlacementGroup GetPlacementGroupById(const std::string &id);
      PlacementGroup GetPlacementGroup(const std::string &name);

    private:
      ObjectID Submit(InvocationSpec &invocation, const ActorCreationOptions &options);

//      std::unordered_map<ActorID, std::unique_ptr<ActorContext>> actor_contexts_;

      absl::Mutex actor_contexts_mutex_;

      std::unordered_map<std::string, ActorID> named_actors_ GUARDED_BY(named_actors_mutex_);
      mutable absl::Mutex named_actors_mutex_;

      std::unique_ptr<boost::asio::thread_pool> thread_pool_;

      LocalModeMcsRuntime &local_mode_mcs_tuntime_;

      std::unordered_map<std::string, mcs::PlacementGroup> placement_groups_;
    };
  }  // namespace internal
}  // namespace mcs
