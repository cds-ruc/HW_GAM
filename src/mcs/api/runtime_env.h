//
// Created by lianyu on 2023/1/9.
//

#pragma once
#include <mcs/api/mcs_exception.h>

#include <string>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace mcs {

/// This class provides interfaces of setting runtime environments for job/actor/task.
  class RuntimeEnv {
  public:
    /// Set a runtime env field by name and Object.
    /// \param[in] name The runtime env plugin name.
    /// \param[in] value An object with primitive data type or jsonable type of
    /// nlohmann/json.
    template <typename T>
    void Set(const std::string &name, const T &value);

    /// Get the object of a runtime env field.
    /// \param[in] name The runtime env plugin name.
    template <typename T>
    T Get(const std::string &name) const;

    /// Set a runtime env field by name and json string.
    /// \param[in] name The runtime env plugin name.
    /// \param[in] json_str A json string represents the runtime env field.
    void SetJsonStr(const std::string &name, const std::string &json_str);

    /// Get the json string of a runtime env field.
    /// \param[in] name The runtime env plugin name.
    std::string GetJsonStr(const std::string &name) const;

    /// Whether a field is contained.
    /// \param[in] name The runtime env plugin name.
    bool Contains(const std::string &name) const;

    /// Remove a field by name.
    /// \param[in] name The runtime env plugin name.
    /// \return true if remove an existing field, otherwise false.
    bool Remove(const std::string &name);

    /// Whether the runtime env is empty.
    bool Empty() const;

    /// Serialize the runtime env to string.
    std::string Serialize() const;

    /// Serialize the runtime env to RuntimeEnvInfo.
    std::string SerializeToRuntimeEnvInfo() const;

    /// Deserialize the runtime env from string.
    /// \return The deserialized RuntimeEnv instance.
    static RuntimeEnv Deserialize(const std::string &serialized_runtime_env);

  private:
    json fields_;
  };

// --------- inline implementation ------------

  template <typename T>
  inline void RuntimeEnv::Set(const std::string &name, const T &value) {
    try {
      json value_j = value;
      fields_[name] = value_j;
    } catch (std::exception &e) {
      throw mcs::internal::McsRuntimeEnvException("Failed to set the field " + name + ": " +
                                                  e.what());
    }
  }

  template <typename T>
  inline T RuntimeEnv::Get(const std::string &name) const {
    if (!Contains(name)) {
      throw mcs::internal::McsRuntimeEnvException("The field " + name + " not found.");
    }
    try {
      return fields_[name].get<T>();
    } catch (std::exception &e) {
      throw mcs::internal::McsRuntimeEnvException("Failed to get the field " + name + ": " +
                                                  e.what());
    }
  }

}  // namespace mcs
