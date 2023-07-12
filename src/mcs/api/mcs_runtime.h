//
// Created by lianyu on 2023/1/5.
//

#pragma once

#include "task_options.h"
#include "common_types.h"
#include <mcs/api/xlang_function.h>

#include <cstdint>
#include <memory>
#include "msgpack.hpp"
#include <typeinfo>
#include <vector>

namespace mcs {
  namespace internal {
    struct RemoteFunctionHolder {
      RemoteFunctionHolder() = default;
      RemoteFunctionHolder(const std::string &module_name,
                           const std::string &function_name,
                           const std::string &class_name = "",
                           LangType lang_type = LangType::CPP) {
        this->module_name = module_name;
        this->function_name = function_name;
        this->class_name = class_name;
        this->lang_type = lang_type;
      }
      RemoteFunctionHolder(std::string func_name) {
        if (func_name.empty()) {
          throw McsException(
                  "Function not found. Please use RAY_REMOTE to register this function.");
        }
        function_name = std::move(func_name);
      }

      std::string module_name;
      std::string function_name;
      std::string class_name;
      LangType lang_type = LangType::CPP;
    };

    class McsRuntime {
    public:
      // virtual std::string Put(std::shared_ptr<msgpack::sbuffer> data) = 0;
      virtual std::shared_ptr<msgpack::sbuffer> Get(const std::string &id) = 0;

      // virtual std::vector<std::shared_ptr<msgpack::sbuffer>> Get(
      //         const std::vector<std::string> &ids) = 0;

//      virtual std::vector<bool> Wait(const std::vector<std::string> &ids,
//                                     int num_objects,
//                                     int timeout_ms) = 0;
//
      virtual std::string Call(const RemoteFunctionHolder &remote_function_holder,
                               std::vector<TaskArg> &args,
                               const CallOptions &task_options) = 0;
//      virtual std::string CreateActor(const RemoteFunctionHolder &remote_function_holder,
//                                      std::vector<TaskArg> &args,
//                                      const ActorCreationOptions &create_options) = 0;
//      virtual std::string CallActor(const RemoteFunctionHolder &remote_function_holder,
//                                    const std::string &actor,
//                                    std::vector<TaskArg> &args,
//                                    const CallOptions &call_options) = 0;
      virtual void AddLocalReference(const std::string &id) = 0;
      virtual void RemoveLocalReference(const std::string &id) = 0;
//      virtual std::string GetActorId(const std::string &actor_name,
//                                     const std::string &mcs_namespace) = 0;
//      virtual void KillActor(const std::string &str_actor_id, bool no_restart) = 0;
//      virtual void ExitActor() = 0;
//      virtual mcs::PlacementGroup CreatePlacementGroup(
//              const mcs::PlacementGroupCreationOptions &create_options) = 0;
//      virtual void RemovePlacementGroup(const std::string &group_id) = 0;
//      virtual bool WaitPlacementGroupReady(const std::string &group_id,
//                                           int64_t timeout_seconds) = 0;
//      virtual bool WasCurrentActorRestarted() = 0;
//      virtual std::vector<PlacementGroup> GetAllPlacementGroups() = 0;
//      virtual PlacementGroup GetPlacementGroupById(const std::string &id) = 0;
//      virtual PlacementGroup GetPlacementGroup(const std::string &name) = 0;
      virtual bool IsLocalMode() { return false; }
//      virtual std::string GetNamespace() = 0;
      virtual std::string SerializeActorHandle(const std::string &actor_id) = 0;
    //  virtual std::string DeserializeAndRegisterActorHandle(
    //          const std::string &serialized_actor_handle) = 0;
    };
  } //namespace internal
} //namespace mcs
