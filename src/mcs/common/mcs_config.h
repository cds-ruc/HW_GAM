//
// Created by root on 1/11/23.
//

#pragma once

#include <cstdint>
#include <sstream>
#include <string>
#include <thread>

#include "absl/strings/escaping.h"
#include "mcs/util/logging.h"

template <typename T>
T ConvertValue(const std::string &type_string, const std::string &value) {
    std::istringstream stream(value);
    T parsed_value;
    stream >> parsed_value;
    MCS_CHECK(!value.empty() && stream.eof())
            << "Cannot parse \"" << value << "\" to " << type_string;
    return parsed_value;
}

template <>
inline std::string ConvertValue<std::string>(const std::string &type_string,
                                             const std::string &value) {
    return value;
}

template <>
inline bool ConvertValue<bool>(const std::string &type_string, const std::string &value) {
    auto new_value = absl::AsciiStrToLower(value);
    return new_value == "true" || new_value == "1";
}

class McsConfig {
/// -----------Include mcs_config_def.h to define config items.----------------
/// A helper macro that defines a config item.
/// In particular, this generates a private field called `name_` and a public getter
/// method called `name()` for a given config item.
///
/// Configs defined in this way can be overridden by setting the env variable
/// MCS_{name}=value where {name} is the variable name.
///
/// \param type Type of the config item.
/// \param name Name of the config item.
/// \param default_value Default value of the config item.
#define MCS_CONFIG(type, name, default_value)                       \
 private:                                                           \
  type name##_ = ReadEnv<type>("MCS_" #name, #type, default_value); \
                                                                    \
 public:                                                            \
  inline type &name() { return name##_; }

#include "mcs/common/mcs_config_def.h"

/// -----------Include mcs_internal_flag_def.h to define internal flags-------
/// MCS_INTERNAL_FLAG defines McsConfig fields similar to the MCS_CONFIG macro.
/// The difference is that MCS_INTERNAL_FLAG is intended for Mcs internal
/// settings that users should not modify.
#define MCS_INTERNAL_FLAG MCS_CONFIG

#include "mcs/common/mcs_internal_flag_def.h"

#undef MCS_INTERNAL_FLAG
#undef MCS_CONFIG

public:
    static McsConfig &instance();

    void initialize(const std::string &config_list);

private:
    template <typename T>
    T ReadEnv(const std::string &name, const std::string &type_string, T default_value) {
        auto value = std::getenv(name.c_str());
        if (value == nullptr) {
            return default_value;
        } else {
            return ConvertValue<T>(type_string, value);
        }
    }
};
