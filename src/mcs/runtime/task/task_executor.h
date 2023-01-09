//
// Created by lianyu on 2023/1/10.
//

#pragma once

#include <mcs/api/function_manager.h>
#include <mcs/api/serializer.h>

#include <boost/dll.hpp>
#include <memory>

#include "absl/synchronization/mutex.h"
#include "invocation_spec.h"
#include "mcs/common/id.h"
//#include "mcs/common/task/task_spec.h"
//#include "mcs/core_worker/common.h"

namespace mcs {

  namespace internal {

/// Execute remote functions by networking stream.
    msgpack::sbuffer TaskExecutionHandler(const std::string &func_name,
                                          const ArgsBufferList &args_buffer,
                                          msgpack::sbuffer *actor_ptr);

    BOOST_DLL_ALIAS(internal::TaskExecutionHandler, TaskExecutionHandler);

    FunctionManager &GetFunctionManager();
    BOOST_DLL_ALIAS(internal::GetFunctionManager, GetFunctionManager);

    std::pair<const RemoteFunctionMap_t &, const RemoteMemberFunctionMap_t &>
    GetRemoteFunctions();
    BOOST_DLL_ALIAS(internal::GetRemoteFunctions, GetRemoteFunctions);

    void InitMcsRuntime(std::shared_ptr<McsRuntime> runtime);
    BOOST_DLL_ALIAS(internal::InitMcsRuntime, InitMcsRuntime);
  }  // namespace internal

  namespace internal {

//    using mcs::core::McsFunction;

    class AbstractMcsRuntime;

    class ActorContext {
    public:
      std::shared_ptr<msgpack::sbuffer> current_actor = nullptr;

      std::shared_ptr<absl::Mutex> actor_mutex;

      ActorContext() { actor_mutex = std::shared_ptr<absl::Mutex>(new absl::Mutex); }
    };

    class TaskExecutor {
    public:
      TaskExecutor() = default;

//      static void Invoke(
//              const TaskSpecification &task_spec,
//              std::shared_ptr<msgpack::sbuffer> actor,
//              AbstractMcsRuntime *runtime,
//              std::unordered_map<ActorID, std::unique_ptr<ActorContext>> &actor_contexts,
//              absl::Mutex &actor_contexts_mutex);

//      static Status ExecuteTask(
//              const rpc::Address &caller_address,
//              mcs::TaskType task_type,
//              const std::string task_name,
//              const McsFunction &mcs_function,
//              const std::unordered_map<std::string, double> &required_resources,
//              const std::vector<std::shared_ptr<mcs::McsObject>> &args,
//              const std::vector<rpc::ObjectReference> &arg_refs,
//              const std::string &debugger_breakpoint,
//              const std::string &serialized_retry_exception_allowlist,
//              std::vector<std::pair<ObjectID, std::shared_ptr<McsObject>>> *returns,
//              std::vector<std::pair<ObjectID, std::shared_ptr<McsObject>>> *dynamic_returns,
//              std::shared_ptr<mcs::LocalMemoryBuffer> &creation_task_exception_pb_bytes,
//              bool *is_retryable_error,
//              bool *is_application_error,
//              const std::vector<ConcurrencyGroup> &defined_concurrency_groups,
//              const std::string name_of_concurrency_group_to_execute,
//              bool is_reattempt);

      virtual ~TaskExecutor(){};

    private:
      static std::shared_ptr<msgpack::sbuffer> current_actor_;
    };
  }  // namespace internal
}  // namespace mcs

