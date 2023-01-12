//
// Created by root on 1/11/23.
//

#include "mcs/common/mcs_config.h"

#include <sstream>
#include <typeinfo>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

McsConfig &McsConfig::instance() {
    static McsConfig config;
    return config;
}

void McsConfig::initialize(const std::string &config_list) {
#define MCS_CONFIG(type, name, default_value) \
  name##_ = ReadEnv<type>("MCS_" #name, #type, default_value);

#include "mcs/common/mcs_config_def.h"
#undef MCS_CONFIG

    if (config_list.empty()) {
        return;
    }

    try {
        // Parse the configuration list.
        json config_map = json::parse(config_list);

/// -----------Include mcs_config_def.h to set config items.-------------------
/// A helper macro that helps to set a value to a config item.
#define MCS_CONFIG(type, name, default_value) \
  if (pair.key() == #name) {                  \
    name##_ = pair.value().get<type>();       \
    continue;                                 \
  }

        for (const auto &pair : config_map.items()) {
            // We use a big chain of if else statements because C++ doesn't allow
            // switch statements on strings.
#include "mcs/common/mcs_config_def.h"
            // "mcs/common/mcs_internal_flag_def.h" is intentionally not included,
            // because it contains Mcs internal settings.
            MCS_LOG(FATAL) << "Received unexpected config parameter " << pair.key();
        }

/// ---------------------------------------------------------------------
#undef MCS_CONFIG

        if (MCS_LOG_ENABLED(DEBUG)) {
            std::ostringstream oss;
            oss << "McsConfig is initialized with: ";
            for (auto const &pair : config_map.items()) {
                oss << pair.key() << "=" << pair.value() << ",";
            }
            MCS_LOG(DEBUG) << oss.str();
        }
    } catch (json::exception &ex) {
        MCS_LOG(FATAL) << "Failed to initialize McsConfig: " << ex.what()
                       << " The config string is: " << config_list;
    }
}
