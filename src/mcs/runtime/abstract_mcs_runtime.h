//
// Created by lianyu on 2023/1/4.
//

#pragma once

#pragma once

#include <mcs/api/mcs_runtime.h>

#include "msgpack.hpp"
#include <mutex>

#include "mcs/core/config_internal.h"
#include "mcs/util/process_helper.h"
#include "mcs/runtime/object/object_store.h"
#include "mcs/runtime/task/task_executor.h"
#include "mcs/runtime/task/task_submitter.h"
#include "mcs/common/id.h"
//#include "mcs/core_worker/context.h"
//#include "mcs/core_worker/core_worker.h"

namespace mcs {
  namespace internal {

//    using mcs::core::WorkerContext;

    class McsIntentionalSystemExitException : public McsException {
    public:
      McsIntentionalSystemExitException(const std::string &msg) : McsException(msg){};
    };

    class AbstractMcsRuntime : public McsRuntime {
    public:
      virtual ~AbstractMcsRuntime(){};

//      void Put(std::shared_ptr<msgpack::sbuffer> data, ObjectID *object_id);
//
//      void Put(std::shared_ptr<msgpack::sbuffer> data, const ObjectID &object_id);
//
//      void Put(mcs::rpc::ErrorType type, const ObjectID &object_id);
//
//      std::string Put(std::shared_ptr<msgpack::sbuffer> data);
//
      std::shared_ptr<msgpack::sbuffer> Get(const std::string &id);
//
//      std::vector<std::shared_ptr<msgpack::sbuffer>> Get(const std::vector<std::string> &ids);
//
//      std::vector<bool> Wait(const std::vector<std::string> &ids,
//                             int num_objects,
//                             int timeout_ms);
//
      std::string Call(const RemoteFunctionHolder &remote_function_holder,
                       std::vector<mcs::internal::TaskArg> &args,
                       const CallOptions &task_options);
//
//      std::string CreateActor(const RemoteFunctionHolder &remote_function_holder,
//                              std::vector<mcs::internal::TaskArg> &args,
//                              const ActorCreationOptions &create_options);
//
//      std::string CallActor(const RemoteFunctionHolder &remote_function_holder,
//                            const std::string &actor,
//                            std::vector<mcs::internal::TaskArg> &args,
//                            const CallOptions &call_options);
//
      void AddLocalReference(const std::string &id);

      void RemoveLocalReference(const std::string &id);
//
//      std::string GetActorId(const std::string &actor_name, const std::string &mcs_namespace);
//
//      void KillActor(const std::string &str_actor_id, bool no_restart);
//
//      void ExitActor();
//
//      mcs::PlacementGroup CreatePlacementGroup(
//              const mcs::PlacementGroupCreationOptions &create_options);
//      void RemovePlacementGroup(const std::string &group_id);
//      bool WaitPlacementGroupReady(const std::string &group_id, int64_t timeout_seconds);
//
//      const TaskID &GetCurrentTaskId();
//
      const JobID &GetCurrentJobID();
//
//      const ActorID &GetCurrentActorID();
//
//      virtual const WorkerContext &GetWorkerContext() = 0;
//
      static std::shared_ptr<AbstractMcsRuntime> GetInstance();
      static std::shared_ptr<AbstractMcsRuntime> DoInit();
//
      static void DoShutdown();
//
//      const std::unique_ptr<mcs::gcs::GlobalStateAccessor> &GetGlobalStateAccessor();
//
//      bool WasCurrentActorRestarted();
//
//      virtual std::vector<PlacementGroup> GetAllPlacementGroups();
//      virtual PlacementGroup GetPlacementGroupById(const std::string &id);
//      virtual PlacementGroup GetPlacementGroup(const std::string &name);
//
//      std::string GetNamespace();
      std::string SerializeActorHandle(const std::string &actor_id);
//      std::string DeserializeAndRegisterActorHandle(
//              const std::string &serialized_actor_handle);
//
    protected:
      std::unique_ptr<TaskSubmitter> task_submitter_;
//      std::unique_ptr<TaskExecutor> task_executor_;
//      std::unique_ptr<ObjectStore> object_store_;
//      std::unique_ptr<mcs::gcs::GlobalStateAccessor> global_state_accessor_;
//
    private:
      static std::shared_ptr<AbstractMcsRuntime> abstract_mcs_runtime_;
//      void Execute(const TaskSpecification &task_spec);
//      PlacementGroup GeneratePlacementGroup(const std::string &str);
    };
  }  // namespace internal
}  // namespace mcs
