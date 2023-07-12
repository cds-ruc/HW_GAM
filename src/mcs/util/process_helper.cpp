//
// Created by lianyu on 2023/1/5.
//

#include "process_helper.h"

#include "mcs/core/process.h"
#include "logging.h"

namespace mcs {
  namespace internal {
    // using mcs::core::WorkerType;

    void ProcessHelper::StartMcsNode(const int port,
                                     const std::vector <std::string> &head_args) {
      std::vector <std::string> cmdargs({"./mcs"});
//      std::vector <std::string> cmdargs({"mcs",
//                                         "start",
//                                         "--head",
//                                         "--port",
//                                         std::to_string(port),
//                                         "--node-ip-address",
//                                         GetNodeIpAddress()});
      if (!head_args.empty()) {
        cmdargs.insert(cmdargs.end(), head_args.begin(), head_args.end());
      }
//      MCS_LOG(INFO) << CreateCommandLine(cmdargs);
      auto spawn_result = Process::Spawn(cmdargs, true);
      MCS_CHECK(!spawn_result.second);
      spawn_result.first.Wait();
      return;
    }

    void ProcessHelper::StopMcsNode() {

    }

    void ProcessHelper::McsStart(CoreWorkerOptions::TaskExecutionCallback callback) {
//       std::string bootstrap_ip = ConfigInternal::Instance().bootstrap_ip;
//       int bootstrap_port = ConfigInternal::Instance().bootstrap_port;

//       if (ConfigInternal::Instance().worker_type == WorkerType::DRIVER &&
//           bootstrap_ip.empty()) {
//         bootstrap_ip = "127.0.0.1";
//         StartMcsNode(bootstrap_port,
//                      ConfigInternal::Instance().head_args);
//       }
//       if (bootstrap_ip == "127.0.0.1") {
// //        bootstrap_ip = GetNodeIpAddress();
//       }

//
//  std::string bootstrap_address = bootstrap_ip + ":" + std::to_string(bootstrap_port);
//  std::string node_ip = ConfigInternal::Instance().node_ip_address;
//  if (node_ip.empty()) {
//    if (!bootstrap_ip.empty()) {
//      node_ip = GetNodeIpAddress(bootstrap_address);
//    } else {
//      node_ip = GetNodeIpAddress();
//    }
//  }
//
//  if (ConfigInternal::Instance().worker_type == WorkerType::DRIVER) {
//    std::string node_to_connect;
//    auto status =
//            global_state_accessor->GetNodeToConnectForDriver(node_ip, &node_to_connect);
////    MCS_CHECK_OK(status);
//    mcs::rpc::GcsNodeInfo node_info;
//    node_info.ParseFromString(node_to_connect);
//    ConfigInternal::Instance().mcslet_socket_name = node_info.mcslet_socket_name();
//    ConfigInternal::Instance().plasma_store_socket_name =
//            node_info.object_store_socket_name();
//    ConfigInternal::Instance().node_manager_port = node_info.node_manager_port();
//  }
//  MCS_CHECK(!ConfigInternal::Instance().mcslet_socket_name.empty());
//  MCS_CHECK(!ConfigInternal::Instance().plasma_store_socket_name.empty());
//  MCS_CHECK(ConfigInternal::Instance().node_manager_port > 0);
//
//  if (ConfigInternal::Instance().worker_type == WorkerType::DRIVER) {
//    auto session_dir = *global_state_accessor->GetInternalKV("session", "session_dir");
//    ConfigInternal::Instance().UpdateSessionDir(session_dir);
//  }
//
//  gcs::GcsClientOptions gcs_options = gcs::GcsClientOptions(bootstrap_address);
//
//  CoreWorkerOptions options;
//  options.worker_type = ConfigInternal::Instance().worker_type;
//  options.language = Language::CPP;
//  options.store_socket = ConfigInternal::Instance().plasma_store_socket_name;
//  options.mcslet_socket = ConfigInternal::Instance().mcslet_socket_name;
//  if (options.worker_type == WorkerType::DRIVER) {
//    if (!ConfigInternal::Instance().job_id.empty()) {
//      options.job_id = JobID::FromHex(ConfigInternal::Instance().job_id);
//    } else {
//      options.job_id = global_state_accessor->GetNextJobID();
//    }
//  }
//  options.gcs_options = gcs_options;
//  options.enable_logging = true;
//  options.log_dir = ConfigInternal::Instance().logs_dir;
//  options.install_failure_signal_handler = true;
//  options.node_ip_address = node_ip;
//  options.node_manager_port = ConfigInternal::Instance().node_manager_port;
//  options.mcslet_ip_address = node_ip;
//  options.driver_name = "cpp_worker";
//  options.metrics_agent_port = -1;
//  options.task_execution_callback = callback;
//  options.startup_token = ConfigInternal::Instance().startup_token;
//  options.runtime_env_hash = ConfigInternal::Instance().runtime_env_hash;
//  rpc::JobConfig job_config;
//  job_config.set_default_actor_lifetime(
//          ConfigInternal::Instance().default_actor_lifetime);
//
//  for (const auto &path : ConfigInternal::Instance().code_search_path) {
//    job_config.add_code_search_path(path);
//  }
//  job_config.set_mcs_namespace(ConfigInternal::Instance().mcs_namespace);
//  if (ConfigInternal::Instance().runtime_env) {
//    job_config.mutable_runtime_env_info()->set_serialized_runtime_env(
//            ConfigInternal::Instance().runtime_env->Serialize());
//  }
//  if (ConfigInternal::Instance().job_config_metadata.size()) {
//    auto metadata_ptr = job_config.mutable_metadata();
//    for (const auto &it : ConfigInternal::Instance().job_config_metadata) {
//      (*metadata_ptr)[it.first] = it.second;
//    }
//  }
//  std::string serialized_job_config;
//  MCS_CHECK(job_config.SerializeToString(&serialized_job_config));
//  options.serialized_job_config = serialized_job_config;
//  CoreWorkerProcess::Initialize(options);
    }

    void ProcessHelper::McsStop() {
//  CoreWorkerProcess::Shutdown();
//  if (ConfigInternal::Instance().bootstrap_ip.empty()) {
//    StopMcsNode();
//  }
    }

  } // namespace internal
} // namespace mcs