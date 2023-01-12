//
// Created by root on 1/12/23.
//

#pragma once

#include <vector>
#include "memory_store.h"
#include "mcs/runtime/local_mode_mcs_runtime.h"
#include "object_store.h"

namespace mcs {
    namespace internal {

        using mcs::core::CoreWorkerMemoryStore;

        class LocalModeObjectStore : public ObjectStore {
        public:
            LocalModeObjectStore(LocalModeMcsRuntime &local_mode_mcs_tuntime);

//            std::vector<bool> Wait(const std::vector<ObjectID> &ids,
//                                   int num_objects,
//                                   int timeout_ms);

            void AddLocalReference(const std::string &id);

            void RemoveLocalReference(const std::string &id);

        private:
            void PutRaw(std::shared_ptr<msgpack::sbuffer> data, ObjectID *object_id);

            void PutRaw(std::shared_ptr<msgpack::sbuffer> data, const ObjectID &object_id);

            std::shared_ptr<msgpack::sbuffer> GetRaw(const ObjectID &object_id, int timeout_ms);

            std::vector<std::shared_ptr<msgpack::sbuffer>> GetRaw(const std::vector<ObjectID> &ids,
                                                                  int timeout_ms);

            std::unique_ptr<CoreWorkerMemoryStore> memory_store_;
            LocalModeMcsRuntime &local_mode_mcs_tuntime_;
        };

    }  // namespace internal
}  // namespace mcs
