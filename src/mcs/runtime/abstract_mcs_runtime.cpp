//
// Created by lianyu on 2023/1/6.
//


#include "abstract_mcs_runtime.h"

#include "mcs/api.h"
#include "mcs/api/mcs_exception.h"
#include "mcs/util/logging.h"

#include <cassert>

#include "mcs/core/config_internal.h"
#include "mcs/util/function_helper.h"
#include "local_mode_mcs_runtime.h"
//#include "native_mcs_runtime.h"

#include "mcs/runtime/task/invocation_spec.h"

namespace mcs {

  namespace internal {
    msgpack::sbuffer PackError(std::string error_msg) {
      msgpack::sbuffer sbuffer;
      msgpack::packer<msgpack::sbuffer> packer(sbuffer);
      packer.pack(msgpack::type::nil_t());
//      packer.pack(std::make_tuple((int)mcs::rpc::ErrorType::TASK_EXECUTION_EXCEPTION,
//                                  std::move(error_msg)));

      return sbuffer;
    }
  }  // namespace internal
  namespace internal {

//    using mcs::core::CoreWorkerProcess;
    using mcs::core::WorkerType;

    std::shared_ptr<AbstractMcsRuntime> AbstractMcsRuntime::abstract_mcs_runtime_ = nullptr;

    std::shared_ptr<AbstractMcsRuntime> AbstractMcsRuntime::DoInit() {
      std::shared_ptr<AbstractMcsRuntime> runtime;
      if (ConfigInternal::Instance().run_mode == RunMode::SINGLE_PROCESS) {
        runtime = std::shared_ptr<AbstractMcsRuntime>(new LocalModeMcsRuntime());
      } else {
//        ProcessHelper::GetInstance().McsStart(TaskExecutor::ExecuteTask);
//        runtime = std::shared_ptr<AbstractMcsRuntime>(new NativeMcsRuntime());
//        MCS_LOG(INFO) << "Native mcs runtime started.";
      }
      MCS_CHECK(runtime);
      internal::McsRuntimeHolder::Instance().Init(runtime);
      if (ConfigInternal::Instance().worker_type == WorkerType::WORKER) {
        // Load functions from code search path.
        FunctionHelper::GetInstance().LoadFunctionsFromPaths(
                ConfigInternal::Instance().code_search_path);
      }
      abstract_mcs_runtime_ = runtime;
      return runtime;
    }

    std::shared_ptr<AbstractMcsRuntime> AbstractMcsRuntime::GetInstance() {
      return abstract_mcs_runtime_;
    }

    void AbstractMcsRuntime::DoShutdown() {
      abstract_mcs_runtime_ = nullptr;
      if (ConfigInternal::Instance().run_mode == RunMode::CLUSTER) {
        ProcessHelper::GetInstance().McsStop();
      }
    }
//
//    void AbstractMcsRuntime::Put(std::shared_ptr<msgpack::sbuffer> data,
//                                 ObjectID *object_id) {
//      object_store_->Put(data, object_id);
//    }
//
//    void AbstractMcsRuntime::Put(std::shared_ptr<msgpack::sbuffer> data,
//                                 const ObjectID &object_id) {
//      object_store_->Put(data, object_id);
//    }
//
//    std::string AbstractMcsRuntime::Put(std::shared_ptr<msgpack::sbuffer> data) {
//      ObjectID object_id;
//      object_store_->Put(data, &object_id);
//      return object_id.Binary();
//    }
//
    std::shared_ptr<msgpack::sbuffer> AbstractMcsRuntime::Get(const std::string &object_id) {
//      return object_store_->Get(ObjectID::FromBinary(object_id), -1);
      return nullptr;
    }

//
//    inline static std::vector<ObjectID> StringIDsToObjectIDs(
//            const std::vector<std::string> &ids) {
//      std::vector<ObjectID> object_ids;
//      for (std::string id : ids) {
//        object_ids.push_back(ObjectID::FromBinary(id));
//      }
//      return object_ids;
//    }
//
//    std::vector<std::shared_ptr<msgpack::sbuffer>> AbstractMcsRuntime::Get(
//            const std::vector<std::string> &ids) {
//      return object_store_->Get(StringIDsToObjectIDs(ids), -1);
//    }
//
//    std::vector<bool> AbstractMcsRuntime::Wait(const std::vector<std::string> &ids,
//                                               int num_objects,
//                                               int timeout_ms) {
//      return object_store_->Wait(StringIDsToObjectIDs(ids), num_objects, timeout_ms);
//    }
//
    std::vector<std::unique_ptr<::mcs::TaskArg>> TransformArgs(
            std::vector<mcs::internal::TaskArg> &args, bool cross_lang) {
      std::vector<std::unique_ptr<::mcs::TaskArg>> mcs_args;
      for (auto &arg : args) {
        std::unique_ptr<::mcs::TaskArg> mcs_arg = nullptr;
        if (arg.buf) {
          auto &buffer = *arg.buf;
          auto memory_buffer = std::make_shared<mcs::LocalMemoryBuffer>(
                  reinterpret_cast<uint8_t *>(buffer.data()), buffer.size(), true);
          std::shared_ptr<Buffer> metadata = nullptr;
//          if (cross_lang) {
//            auto meta_str = arg.meta_str;
//            metadata = std::make_shared<mcs::LocalMemoryBuffer>(
//                    reinterpret_cast<uint8_t *>(const_cast<char *>(meta_str.data())),
//                    meta_str.size(),
//                    true);
//          }
          mcs_arg = absl::make_unique<mcs::TaskArgByValue>(std::make_shared<mcs::McsObject>(
                  memory_buffer, metadata, std::vector<rpc::ObjectReference>()));
        } else {
          MCS_CHECK(arg.id);
//          auto id = ObjectID::FromBinary(*arg.id);
//          auto owner_address = mcs::rpc::Address{};
//          if (ConfigInternal::Instance().run_mode == RunMode::CLUSTER) {
//            auto &core_worker = CoreWorkerProcess::GetCoreWorker();
//            owner_address = core_worker.GetOwnerAddressOrDie(id);
//          }
//          mcs_arg = absl::make_unique<mcs::TaskArgByReference>(id,
//                                                               owner_address,
//                  /*call_site=*/"");
        }
        mcs_args.push_back(std::move(mcs_arg));
      }

      return mcs_args;
    }

    InvocationSpec BuildInvocationSpec1(TaskType task_type,
                                        const RemoteFunctionHolder &remote_function_holder,
                                        std::vector<mcs::internal::TaskArg> &args,
                                        const ActorID &actor) {
      InvocationSpec invocation_spec;
      invocation_spec.task_type = task_type;
      invocation_spec.remote_function_holder = remote_function_holder;
      invocation_spec.actor_id = actor;
      invocation_spec.args =
              TransformArgs(args, remote_function_holder.lang_type != LangType::CPP);
      return invocation_spec;
    }

    std::string AbstractMcsRuntime::Call(const RemoteFunctionHolder &remote_function_holder,
                                         std::vector<mcs::internal::TaskArg> &args,
                                         const CallOptions &task_options) {
      auto invocation_spec = BuildInvocationSpec1(
              TaskType::NORMAL_TASK, remote_function_holder, args, ActorID::Nil());
//      return task_submitter_->SubmitTask(invocation_spec, task_options).Binary();
      return "";
    }
//
//    std::string AbstractMcsRuntime::CreateActor(
//            const RemoteFunctionHolder &remote_function_holder,
//            std::vector<mcs::internal::TaskArg> &args,
//            const ActorCreationOptions &create_options) {
//      auto invocation_spec = BuildInvocationSpec1(
//              TaskType::ACTOR_CREATION_TASK, remote_function_holder, args, ActorID::Nil());
//      return task_submitter_->CreateActor(invocation_spec, create_options).Binary();
//    }
//
//    std::string AbstractMcsRuntime::CallActor(
//            const RemoteFunctionHolder &remote_function_holder,
//            const std::string &actor,
//            std::vector<mcs::internal::TaskArg> &args,
//            const CallOptions &call_options) {
//      InvocationSpec invocation_spec{};
//      if (remote_function_holder.lang_type == LangType::PYTHON) {
//        const auto native_actor_handle = CoreWorkerProcess::GetCoreWorker().GetActorHandle(
//                mcs::ActorID::FromBinary(actor));
//        auto function_descriptor = native_actor_handle->ActorCreationTaskFunctionDescriptor();
//        auto typed_descriptor = function_descriptor->As<PythonFunctionDescriptor>();
//        RemoteFunctionHolder func_holder = remote_function_holder;
//        func_holder.module_name = typed_descriptor->ModuleName();
//        func_holder.class_name = typed_descriptor->ClassName();
//        invocation_spec = BuildInvocationSpec1(
//                TaskType::ACTOR_TASK, func_holder, args, ActorID::FromBinary(actor));
//      } else if (remote_function_holder.lang_type == LangType::JAVA) {
//        const auto native_actor_handle = CoreWorkerProcess::GetCoreWorker().GetActorHandle(
//                mcs::ActorID::FromBinary(actor));
//        auto function_descriptor = native_actor_handle->ActorCreationTaskFunctionDescriptor();
//        auto typed_descriptor = function_descriptor->As<JavaFunctionDescriptor>();
//        RemoteFunctionHolder func_holder = remote_function_holder;
//        func_holder.class_name = typed_descriptor->ClassName();
//        invocation_spec = BuildInvocationSpec1(
//                TaskType::ACTOR_TASK, func_holder, args, ActorID::FromBinary(actor));
//      } else {
//        invocation_spec = BuildInvocationSpec1(
//                TaskType::ACTOR_TASK, remote_function_holder, args, ActorID::FromBinary(actor));
//      }
//
//      return task_submitter_->SubmitActorTask(invocation_spec, call_options).Binary();
//    }
//
//    const TaskID &AbstractMcsRuntime::GetCurrentTaskId() {
//      return GetWorkerContext().GetCurrentTaskID();
//    }
//
//    const JobID &AbstractMcsRuntime::GetCurrentJobID() {
//      return GetWorkerContext().GetCurrentJobID();
//    }
//
//    const ActorID &AbstractMcsRuntime::GetCurrentActorID() {
//      return GetWorkerContext().GetCurrentActorID();
//    }
//
    void AbstractMcsRuntime::AddLocalReference(const std::string &id) {
//      if (CoreWorkerProcess::IsInitialized()) {
//        auto &core_worker = CoreWorkerProcess::GetCoreWorker();
//        core_worker.AddLocalReference(ObjectID::FromBinary(id));
//      }
    }

    void AbstractMcsRuntime::RemoveLocalReference(const std::string &id) {
//      if (CoreWorkerProcess::IsInitialized()) {
//        auto &core_worker = CoreWorkerProcess::GetCoreWorker();
//        core_worker.RemoveLocalReference(ObjectID::FromBinary(id));
//      }
    }
//
//    std::string AbstractMcsRuntime::GetActorId(const std::string &actor_name,
//                                               const std::string &mcs_namespace) {
//      auto actor_id = task_submitter_->GetActor(actor_name, mcs_namespace);
//      if (actor_id.IsNil()) {
//        return "";
//      }
//
//      return actor_id.Binary();
//    }
//
//    void AbstractMcsRuntime::KillActor(const std::string &str_actor_id, bool no_restart) {
//      auto &core_worker = CoreWorkerProcess::GetCoreWorker();
//      mcs::ActorID actor_id = mcs::ActorID::FromBinary(str_actor_id);
//      Status status = core_worker.KillActor(actor_id, true, no_restart);
//      if (!status.ok()) {
//        throw McsException(status.message());
//      }
//    }
//
//    void AbstractMcsRuntime::ExitActor() {
//      auto &core_worker = CoreWorkerProcess::GetCoreWorker();
//      if (ConfigInternal::Instance().worker_type != WorkerType::WORKER ||
//          core_worker.GetActorId().IsNil()) {
//        throw std::logic_error("This shouldn't be called on a non-actor worker.");
//      }
//      throw McsIntentionalSystemExitException("SystemExit");
//    }
//
//    const std::unique_ptr<mcs::gcs::GlobalStateAccessor>
//    &AbstractMcsRuntime::GetGlobalStateAccessor() {
//      return global_state_accessor_;
//    }
//
//    bool AbstractMcsRuntime::WasCurrentActorRestarted() {
//      if (ConfigInternal::Instance().run_mode == RunMode::SINGLE_PROCESS) {
//        return false;
//      }
//
//      const auto &actor_id = GetCurrentActorID();
//      auto byte_ptr = global_state_accessor_->GetActorInfo(actor_id);
//      if (byte_ptr == nullptr) {
//        return false;
//      }
//
//      rpc::ActorTableData actor_table_data;
//      bool r = actor_table_data.ParseFromString(*byte_ptr);
//      if (!r) {
//        throw McsException("Received invalid protobuf data from GCS.");
//      }
//
//      return actor_table_data.num_restarts() != 0;
//    }
//
//    mcs::PlacementGroup AbstractMcsRuntime::CreatePlacementGroup(
//            const mcs::PlacementGroupCreationOptions &create_options) {
//      return task_submitter_->CreatePlacementGroup(create_options);
//    }
//
//    void AbstractMcsRuntime::RemovePlacementGroup(const std::string &group_id) {
//      return task_submitter_->RemovePlacementGroup(group_id);
//    }
//
//    bool AbstractMcsRuntime::WaitPlacementGroupReady(const std::string &group_id,
//                                                     int64_t timeout_seconds) {
//      return task_submitter_->WaitPlacementGroupReady(group_id, timeout_seconds);
//    }
//
//    PlacementGroup AbstractMcsRuntime::GeneratePlacementGroup(const std::string &str) {
//      rpc::PlacementGroupTableData pg_table_data;
//      bool r = pg_table_data.ParseFromString(str);
//      if (!r) {
//        throw McsException("Received invalid protobuf data from GCS.");
//      }
//
//      PlacementGroupCreationOptions options;
//      options.name = pg_table_data.name();
//      auto &bundles = options.bundles;
//      for (auto &bundle : bundles) {
//        options.bundles.emplace_back(bundle);
//      }
//      options.strategy = PlacementStrategy(pg_table_data.strategy());
//      PlacementGroup group(pg_table_data.placement_group_id(),
//                           std::move(options),
//                           PlacementGroupState(pg_table_data.state()));
//      return group;
//    }
//
//    std::vector<PlacementGroup> AbstractMcsRuntime::GetAllPlacementGroups() {
//      std::vector<std::string> list = global_state_accessor_->GetAllPlacementGroupInfo();
//      std::vector<PlacementGroup> groups;
//      for (auto &str : list) {
//        PlacementGroup group = GeneratePlacementGroup(str);
//        groups.push_back(std::move(group));
//      }
//
//      return groups;
//    }
//
//    PlacementGroup AbstractMcsRuntime::GetPlacementGroupById(const std::string &id) {
//      PlacementGroupID pg_id = PlacementGroupID::FromBinary(id);
//      auto str_ptr = global_state_accessor_->GetPlacementGroupInfo(pg_id);
//      if (str_ptr == nullptr) {
//        return {};
//      }
//      PlacementGroup group = GeneratePlacementGroup(*str_ptr);
//      return group;
//    }
//
//    PlacementGroup AbstractMcsRuntime::GetPlacementGroup(const std::string &name) {
//      // TODO(WangTaoTheTonic): Add namespace support for placement group.
//      auto str_ptr = global_state_accessor_->GetPlacementGroupByName(
//              name, CoreWorkerProcess::GetCoreWorker().GetJobConfig().mcs_namespace());
//      if (str_ptr == nullptr) {
//        return {};
//      }
//      PlacementGroup group = GeneratePlacementGroup(*str_ptr);
//      return group;
//    }
//
//    std::string AbstractMcsRuntime::GetNamespace() {
//      auto &core_worker = CoreWorkerProcess::GetCoreWorker();
//      return core_worker.GetJobConfig().mcs_namespace();
//    }
//
    std::string AbstractMcsRuntime::SerializeActorHandle(const std::string &actor_id) {
//      auto &core_worker = CoreWorkerProcess::GetCoreWorker();
//      std::string output;
//      ObjectID actor_handle_id;
//      auto status = core_worker.SerializeActorHandle(
//              ActorID::FromBinary(actor_id), &output, &actor_handle_id);
//      return output;
//    }
//
//    std::string AbstractMcsRuntime::DeserializeAndRegisterActorHandle(
//            const std::string &serialized_actor_handle) {
//      auto &core_worker = CoreWorkerProcess::GetCoreWorker();
//      return core_worker
//              .DeserializeAndRegisterActorHandle(serialized_actor_handle, ObjectID::Nil())
//              .Binary();
      return "";
    }

  }  // namespace internal
}  // namespace mcs

