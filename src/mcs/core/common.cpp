//
// Created by root on 1/11/23.
//

#include "mcs/core/common.h"

namespace mcs {
    namespace core {

        std::string WorkerTypeString(WorkerType type) {
            // TODO(suquark): Use proto3 utils to get the string.
            if (type == WorkerType::DRIVER) {
                return "driver";
            } else if (type == WorkerType::WORKER) {
                return "worker";
            } else if (type == WorkerType::SPILL_WORKER) {
                return "spill_worker";
            } else if (type == WorkerType::RESTORE_WORKER) {
                return "restore_worker";
            }
            MCS_CHECK(false);
            return "";
        }

        std::string LanguageString(Language language) {
            if (language == Language::PYTHON) {
                return "python";
            } else if (language == Language::JAVA) {
                return "java";
            } else if (language == Language::CPP) {
                return "cpp";
            }
            MCS_CHECK(false);
            return "";
        }

        std::string GenerateCachedActorName(const std::string &ns,
                                            const std::string &actor_name) {
            return ns + "-" + actor_name;
        }

    }  // namespace core
}  // namespace mcs