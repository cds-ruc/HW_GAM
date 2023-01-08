//
// Created by lianyu on 2023/1/6.
//

#pragma once

#include <vector>
#include <unordered_map>
#include <cmath>

#include "mcs_exception.h"

namespace mcs {
  namespace internal {
    // Type of a task.
    enum TaskType {
      // Normal task.
      NORMAL_TASK = 0,
      // Actor creation task.
      ACTOR_CREATION_TASK = 1,
      // Actor task.
      ACTOR_TASK = 2,
      // Driver task.
      DRIVER_TASK = 3,
    };

    inline void CheckTaskOptions(const std::unordered_map<std::string, double> &resources) {
      for (auto &pair : resources) {
        if (pair.first.empty() || pair.second == 0) {
          throw McsException("Resource values should be positive. Specified resource: " +
                             pair.first + " = " + std::to_string(pair.second) + ".");
        }
        // Note: A resource value should be an integer if it is greater than 1.0.
        // e.g. 3.0 is a valid resource value, but 3.5 is not.
        double intpart;
        if (pair.second > 1 && std::modf(pair.second, &intpart) != 0.0) {
          throw McsException(
                  "A resource value should be an integer if it is greater than 1.0. Specified "
                  "resource: " +
                  pair.first + " = " + std::to_string(pair.second) + ".");
        }
      }
    }

  }  // namespace internal

  enum class PlacementStrategy {
    PACK = 0,
    SPREAD = 1,
    STRICT_PACK = 2,
    STRICT_SPREAD = 3,
    UNRECOGNIZED = -1
  };

  enum PlacementGroupState {
    PENDING = 0,
    CREATED = 1,
    REMOVED = 2,
    RESCHEDULING = 3,
    UNRECOGNIZED = -1,
  };

  struct PlacementGroupCreationOptions {
    std::string name;
    std::vector<std::unordered_map<std::string, double>> bundles;
    PlacementStrategy strategy;
  };

  class PlacementGroup {
  public:
    PlacementGroup() = default;
    PlacementGroup(std::string id,
                   PlacementGroupCreationOptions options,
                   PlacementGroupState state = PlacementGroupState::UNRECOGNIZED)
            : id_(std::move(id)), options_(std::move(options)), state_(state) {}
    std::string GetID() const { return id_; }
    std::string GetName() { return options_.name; }
    std::vector<std::unordered_map<std::string, double>> GetBundles() {
      return options_.bundles;
    }
    mcs::PlacementGroupState GetState() { return state_; }
    PlacementStrategy GetStrategy() { return options_.strategy; }
    bool Wait(int timeout_seconds) { return callback_(id_, timeout_seconds); }
    void SetWaitCallbak(std::function<bool(const std::string &, int)> callback) {
      callback_ = std::move(callback);
    }
    bool Empty() const { return id_.empty(); }

  private:
    std::string id_;
    PlacementGroupCreationOptions options_;
    PlacementGroupState state_;
    std::function<bool(const std::string &, int)> callback_;
  };

  namespace internal {

    struct CallOptions {
      std::string name;
      std::unordered_map<std::string, double> resources;
      PlacementGroup group;
      int bundle_index;
      std::string serialized_runtime_env_info;
    };

    struct ActorCreationOptions {
      std::string name;
      std::string mcs_namespace;
      std::unordered_map<std::string, double> resources;
      int max_restarts = 0;
      int max_concurrency = 1;
      PlacementGroup group;
      int bundle_index;
      std::string serialized_runtime_env_info;
    };
  }  // namespace internal

}  // namespace mcs