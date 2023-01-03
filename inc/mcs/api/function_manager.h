//
// Created by hrh on 1/4/23.
//
#pragma once
#include <functional>
#include <unordered_map>
#include <map>

#include "common_types.h"

namespace mcs {
  namespace internal {
/// 管理所有Remote函数
    class FunctionManager {
    public:
      static FunctionManager &Instance() {
        static FunctionManager instance;
        return instance;
      }

      RemoteFunction *GetFunction(const std::string &func_name) {
        auto it = map_invokers_.find(func_name);
        if (it == map_invokers_.end()) {
          return nullptr;
        }

        return &it->second;
      }

      template <typename Function>
      std::enable_if_t<std::is_member_function_pointer<Function>::value, bool>
      RegisterRemoteFunction(std::string const &name, const Function &f) {
        using Self = boost::callable_traits::class_of_t<Function>;
        auto key = std::make_pair(typeid(Self).name(), GetAddress(f));
        auto pair = mem_func_to_key_map_.emplace(std::move(key), name);
        if (!pair.second) {
          throw RayException("Duplicate RAY_REMOTE function: " + name);
        }

        bool ok = RegisterMemberFunc(name, f);
        if (!ok) {
          throw RayException("Duplicate RAY_REMOTE function: " + name);
        }

        return true;
      }

    private:
      FunctionManager() = default;
      ~FunctionManager() = default;
      FunctionManager(const FunctionManager &) = delete;
      FunctionManager(FunctionManager &&) = delete;

      template <typename Function>
      bool RegisterNonMemberFunc(std::string const &name, Function f) {
        return map_invokers_
            .emplace(
                name,
                std::bind(&Invoker<Function>::Apply, std::move(f), std::placeholders::_1))
            .second;
      }

      RemoteFunctionMap_t map_invokers_;
      std::unordered_map<std::string, std::string> func_ptr_to_key_map_;
      std::map<std::pair<std::string, std::string>, std::string> mem_func_to_key_map_;
    };
  } // namespace internal
} // namespace mcs
