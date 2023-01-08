//
// Created by lianyu on 2023/1/6.
//

#pragma once

#include <limits.h>
#include <stdint.h>

/// Length of Mcs full-length IDs in bytes.
constexpr size_t kUniqueIDSize = 28;

/// An ObjectID's bytes are split into the task ID itself and the index of the
/// object's creation. This is the maximum width of the object index in bits.
constexpr int kObjectIdIndexSize = 32;
static_assert(kObjectIdIndexSize % CHAR_BIT == 0,
"ObjectID prefix not a multiple of bytes");

/// Mcslet exit code on plasma store socket error.
constexpr int kMcsletStoreErrorExitCode = 100;

/// Prefix for the object table keys in redis.
constexpr char kObjectTablePrefix[] = "ObjectTable";

constexpr char kWorkerDynamicOptionPlaceholder[] =
        "MCS_WORKER_DYNAMIC_OPTION_PLACEHOLDER";

constexpr char kNodeManagerPortPlaceholder[] = "MCS_NODE_MANAGER_PORT_PLACEHOLDER";

/// Public DNS address which is is used to connect and get local IP.
constexpr char kPublicDNSServerIp[] = "8.8.8.8";
constexpr int kPublicDNSServerPort = 53;

constexpr char kEnvVarKeyJobId[] = "MCS_JOB_ID";
constexpr char kEnvVarKeyMcsletPid[] = "MCS_MCSLET_PID";

/// for cross-langueage serialization
constexpr int kMessagePackOffset = 9;

/// Filename of "shim process" that sets up Python worker environment.
/// Should be kept in sync with SETUP_WORKER_FILENAME in ray_constants.py
constexpr char kSetupWorkerFilename[] = "setup_worker.py";

/// The version of Mcs
constexpr char kMcsVersion[] = "3.0.0.dev0";

