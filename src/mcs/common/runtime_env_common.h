//
// Created by root on 1/11/23.
//

#pragma once
#include <string>

namespace mcs {

// Return whether a string representation of a runtime env represents an empty
// runtime env.  It could either be "" (from the default string value in protobuf),
// or "{}" (from serializing an empty Python dict or a JSON file.)
    bool IsRuntimeEnvEmpty(const std::string &serialized_runtime_env);

// Return whether a string representation of a runtime env info represents an empty
// runtime env info.  It could either be "" (from the default string value in protobuf),
// or "{}" (from serializing an empty Python dict or a JSON file.)
    bool IsRuntimeEnvInfoEmpty(const std::string &serialized_runtime_env_info);

}  // namespace mcs