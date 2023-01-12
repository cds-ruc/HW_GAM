//
// Created by root on 1/11/23.
//

#include "runtime_env_common.h"
namespace mcs {

    bool IsRuntimeEnvEmpty(const std::string &serialized_runtime_env) {
        return serialized_runtime_env == "{}" || serialized_runtime_env == "";
    }

    bool IsRuntimeEnvInfoEmpty(const std::string &serialized_runtime_env_info) {
        return serialized_runtime_env_info == "{}" || serialized_runtime_env_info == "";
    }

}  // namespace ray