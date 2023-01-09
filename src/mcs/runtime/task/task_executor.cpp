//
// Created by lianyu on 2023/1/10.
//
#include "task_executor.h"

#include <mcs/api/common_types.h>

#include <memory>

#include "mcs/util/function_helper.h"
#include "mcs/runtime/abstract_mcs_runtime.h"
//#include "mcs/util/event.h"
//#include "mcs/util/event_label.h"

namespace mcs {

  namespace internal {
/// Execute remote functions by networking stream.
    msgpack::sbuffer TaskExecutionHandler(const std::string &func_name,
                                          const ArgsBufferList &args_buffer,
                                          msgpack::sbuffer *actor_ptr) {
      if (func_name.empty()) {
        throw std::invalid_argument("Task function name is empty");
      }

      msgpack::sbuffer result;
      do {
        if (actor_ptr) {
          auto func_ptr = FunctionManager::Instance().GetMemberFunction(func_name);
          if (func_ptr == nullptr) {
            result = PackError("unknown actor task: " + func_name);
            break;
          }
          result = (*func_ptr)(actor_ptr, args_buffer);
        } else {
          auto func_ptr = FunctionManager::Instance().GetFunction(func_name);
          if (func_ptr == nullptr) {
            result = PackError("unknown function: " + func_name);
            break;
          }
          result = (*func_ptr)(args_buffer);
        }
      } while (0);

      return result;
    }

    auto &init_func_manager = FunctionManager::Instance();

    FunctionManager &GetFunctionManager() { return init_func_manager; }

    std::pair<const RemoteFunctionMap_t &, const RemoteMemberFunctionMap_t &>
    GetRemoteFunctions() {
      return init_func_manager.GetRemoteFunctions();
    }

    void InitMcsRuntime(std::shared_ptr<McsRuntime> runtime) {
      McsRuntimeHolder::Instance().Init(runtime);
    }
  }  // namespace internal

  namespace internal {

//    using mcs::core::CoreWorkerProcess;

    std::shared_ptr<msgpack::sbuffer> TaskExecutor::current_actor_ = nullptr;

/// TODO(qicosmos): Need to add more details of the error messages, such as object id,
/// task id etc.
    std::pair<Status, std::shared_ptr<msgpack::sbuffer>> GetExecuteResult(
            const std::string &func_name,
            const ArgsBufferList &args_buffer,
            msgpack::sbuffer *actor_ptr) {
      try {
        EntryFuntion entry_function;
        if (actor_ptr == nullptr) {
          entry_function = FunctionHelper::GetInstance().GetExecutableFunctions(func_name);
        } else {
          entry_function =
                  FunctionHelper::GetInstance().GetExecutableMemberFunctions(func_name);
        }
        MCS_LOG(DEBUG) << "Get executable function " << func_name << " ok.";
        auto result = entry_function(func_name, args_buffer, actor_ptr);
        MCS_LOG(DEBUG) << "Execute function " << func_name << " ok.";
        return std::make_pair(mcs::Status::OK(),
                              std::make_shared<msgpack::sbuffer>(std::move(result)));
      } catch (McsIntentionalSystemExitException &e) {
        MCS_LOG(ERROR) << "Mcs intentional system exit while executing function(" << func_name
                       << ").";
//        return std::make_pair(mcs::Status::IntentionalSystemExit(""), nullptr);
      } catch (const std::exception &e) {
        auto exception_name =
                std::string(abi::__cxa_demangle(typeid(e).name(), nullptr, nullptr, nullptr));
        std::string err_msg = "An exception was thrown while executing function(" +
                              func_name + "): " + exception_name + ": " + e.what();
        MCS_LOG(ERROR) << err_msg;
//        return std::make_pair(mcs::Status::Invalid(err_msg), nullptr);
      } catch (...) {
        MCS_LOG(ERROR) << "An unknown exception was thrown while executing function("
                       << func_name << ").";
//        return std::make_pair(mcs::Status::UnknownError(std::string("unknown exception")),
//                              nullptr);
      }
    }

//    Status TaskExecutor::ExecuteTask(
//            const rpc::Address &caller_address,
//            mcs::TaskType task_type,
//            const std::string task_name,
//            const McsFunction &mcs_function,
//            const std::unordered_map<std::string, double> &required_resources,
//            const std::vector<std::shared_ptr<mcs::McsObject>> &args_buffer,
//            const std::vector<rpc::ObjectReference> &arg_refs,
//            const std::string &debugger_breakpoint,
//            const std::string &serialized_retry_exception_allowlist,
//            std::vector<std::pair<ObjectID, std::shared_ptr<McsObject>>> *returns,
//            std::vector<std::pair<ObjectID, std::shared_ptr<McsObject>>> *dynamic_returns,
//            std::shared_ptr<mcs::LocalMemoryBuffer> &creation_task_exception_pb_bytes,
//            bool *is_retryable_error,
//            bool *is_application_error,
//            const std::vector<ConcurrencyGroup> &defined_concurrency_groups,
//            const std::string name_of_concurrency_group_to_execute,
//            bool is_reattempt) {
//      MCS_LOG(DEBUG) << "Execute task type: " << TaskType_Name(task_type)
//                     << " name:" << task_name;
//      MCS_CHECK(mcs_function.GetLanguage() == mcs::Language::CPP);
//      auto function_descriptor = mcs_function.GetFunctionDescriptor();
//      MCS_CHECK(function_descriptor->Type() ==
//                mcs::FunctionDescriptorType::kCppFunctionDescriptor);
//      auto typed_descriptor = function_descriptor->As<mcs::CppFunctionDescriptor>();
//      std::string func_name = typed_descriptor->FunctionName();
//      bool cross_lang = !typed_descriptor->Caller().empty();
//      // TODO(Clark): Support retrying application-level errors for C++.
//      // TODO(Clark): Support exception allowlist for retrying application-level
//      // errors for C++.
//      *is_retryable_error = false;
//      *is_application_error = false;
//
//      Status status{};
//      std::shared_ptr<msgpack::sbuffer> data = nullptr;
//      ArgsBufferList mcs_args_buffer;
//      for (size_t i = 0; i < args_buffer.size(); i++) {
//        auto &arg = args_buffer.at(i);
//        msgpack::sbuffer sbuf;
//        if (cross_lang) {
//          sbuf.write((const char *)(arg->GetData()->Data()) + XLANG_HEADER_LEN,
//                     arg->GetData()->Size() - XLANG_HEADER_LEN);
//        } else {
//          sbuf.write((const char *)(arg->GetData()->Data()), arg->GetData()->Size());
//        }
//
//        mcs_args_buffer.push_back(std::move(sbuf));
//      }
//      if (task_type == mcs::TaskType::ACTOR_CREATION_TASK) {
//        std::tie(status, data) = GetExecuteResult(func_name, mcs_args_buffer, nullptr);
//        current_actor_ = data;
//      } else if (task_type == mcs::TaskType::ACTOR_TASK) {
//        if (cross_lang) {
//          MCS_CHECK(!typed_descriptor->ClassName().empty());
//          func_name = std::string("&")
//                  .append(typed_descriptor->ClassName())
//                  .append("::")
//                  .append(typed_descriptor->FunctionName());
//        }
//        MCS_CHECK(current_actor_ != nullptr);
//        std::tie(status, data) =
//                GetExecuteResult(func_name, mcs_args_buffer, current_actor_.get());
//      } else {  // NORMAL_TASK
//        std::tie(status, data) = GetExecuteResult(func_name, mcs_args_buffer, nullptr);
//      }
//
//      std::shared_ptr<mcs::LocalMemoryBuffer> meta_buffer = nullptr;
//      if (!status.ok()) {
//        if (status.IsIntentionalSystemExit()) {
//          return status;
//        } else {
//          MCS_EVENT(ERROR, EL_MCS_CPP_TASK_FAILED)
//                  .WithField("task_type", TaskType_Name(task_type))
//                  .WithField("function_name", func_name)
//                  << "C++ task failed: " << status.ToString();
//        }
//
//        std::string meta_str = std::to_string(mcs::rpc::ErrorType::TASK_EXECUTION_EXCEPTION);
//        meta_buffer = std::make_shared<mcs::LocalMemoryBuffer>(
//                reinterpret_cast<uint8_t *>(&meta_str[0]), meta_str.size(), true);
//        *is_application_error = true;
//
//        msgpack::sbuffer buf;
//        if (cross_lang) {
//          mcs::rpc::McsException mcs_exception{};
//          mcs_exception.set_language(mcs::rpc::Language::CPP);
//          mcs_exception.set_formatted_exception_string(status.ToString());
//          auto msg = mcs_exception.SerializeAsString();
//          buf = Serializer::Serialize(msg.data(), msg.size());
//        } else {
//          std::string msg = status.ToString();
//          buf.write(msg.data(), msg.size());
//        }
//        data = std::make_shared<msgpack::sbuffer>(std::move(buf));
//      }
//
//      if (task_type != mcs::TaskType::ACTOR_CREATION_TASK) {
//        size_t data_size = data->size();
//        auto &result_id = (*returns)[0].first;
//        auto result_ptr = &(*returns)[0].second;
//        int64_t task_output_inlined_bytes = 0;
//
//        if (cross_lang && meta_buffer == nullptr) {
//          meta_buffer = std::make_shared<mcs::LocalMemoryBuffer>(
//                  (uint8_t *)(&METADATA_STR_XLANG[0]), METADATA_STR_XLANG.size(), true);
//        }
//
//        size_t total = cross_lang ? (XLANG_HEADER_LEN + data_size) : data_size;
//        MCS_CHECK_OK(CoreWorkerProcess::GetCoreWorker().AllocateReturnObject(
//                result_id,
//                total,
//                meta_buffer,
//                std::vector<mcs::ObjectID>(),
//                &task_output_inlined_bytes,
//                result_ptr));
//
//        auto result = *result_ptr;
//        if (result != nullptr) {
//          if (result->HasData()) {
//            if (cross_lang) {
//              auto len_buf = Serializer::Serialize(data_size);
//
//              msgpack::sbuffer buffer(XLANG_HEADER_LEN + data_size);
//              buffer.write(len_buf.data(), len_buf.size());
//              for (size_t i = 0; i < XLANG_HEADER_LEN - len_buf.size(); ++i) {
//                buffer.write("", 1);
//              }
//              buffer.write(data->data(), data_size);
//
//              memcpy(result->GetData()->Data(), buffer.data(), buffer.size());
//            } else {
//              memcpy(result->GetData()->Data(), data->data(), data_size);
//            }
//          }
//        }
//
//        MCS_CHECK_OK(CoreWorkerProcess::GetCoreWorker().SealReturnObject(
//                result_id,
//                result,
//                /*generator_id=*/ObjectID::Nil()));
//      } else {
//        if (!status.ok()) {
//          return mcs::Status::CreationTaskError("");
//        }
//      }
//      return mcs::Status::OK();
//    }

//    void TaskExecutor::Invoke(
//            const TaskSpecification &task_spec,
//            std::shared_ptr<msgpack::sbuffer> actor,
//            AbstractMcsRuntime *runtime,
//            std::unordered_map<ActorID, std::unique_ptr<ActorContext>> &actor_contexts,
//            absl::Mutex &actor_contexts_mutex) {
//      ArgsBufferList args_buffer;
//      for (size_t i = 0; i < task_spec.NumArgs(); i++) {
//        if (task_spec.ArgByRef(i)) {
//          const auto &id = task_spec.ArgId(i).Binary();
//          msgpack::sbuffer sbuf;
//          sbuf.write(id.data(), id.size());
//          args_buffer.push_back(std::move(sbuf));
//        } else {
//          msgpack::sbuffer sbuf;
//          sbuf.write((const char *)task_spec.ArgData(i), task_spec.ArgDataSize(i));
//          args_buffer.push_back(std::move(sbuf));
//        }
//      }
//
//      auto function_descriptor = task_spec.FunctionDescriptor();
//      auto typed_descriptor = function_descriptor->As<mcs::CppFunctionDescriptor>();
//
//      std::shared_ptr<msgpack::sbuffer> data;
//      try {
//        if (actor) {
//          auto result = TaskExecutionHandler(
//                  typed_descriptor->FunctionName(), args_buffer, actor.get());
//          data = std::make_shared<msgpack::sbuffer>(std::move(result));
//          runtime->Put(std::move(data), task_spec.ReturnId(0));
//        } else {
//          auto result =
//                  TaskExecutionHandler(typed_descriptor->FunctionName(), args_buffer, nullptr);
//          data = std::make_shared<msgpack::sbuffer>(std::move(result));
//          if (task_spec.IsActorCreationTask()) {
//            std::unique_ptr<ActorContext> actorContext(new ActorContext());
//            actorContext->current_actor = data;
//            absl::MutexLock lock(&actor_contexts_mutex);
//            actor_contexts.emplace(task_spec.ActorCreationId(), std::move(actorContext));
//          } else {
//            runtime->Put(std::move(data), task_spec.ReturnId(0));
//          }
//        }
//      } catch (std::exception &e) {
//        auto result = PackError(e.what());
//        auto data = std::make_shared<msgpack::sbuffer>(std::move(result));
//        runtime->Put(std::move(data), task_spec.ReturnId(0));
//      }
//    }

  }  // namespace internal
}  // namespace mcs