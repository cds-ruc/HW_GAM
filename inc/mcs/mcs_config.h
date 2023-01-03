//
// Created by hrh on 1/4/23.
//

class RayConfig {
public:
  // The address of the Ray cluster to connect to.
  // If not provided, it will be initialized from environment variable "RAY_ADDRESS" by
  // default.
  std::string address = "";

  // Whether or not to run this application in a local mode. This is used for debugging.
  bool local_mode = false;

  // An array of directories or dynamic library files that specify the search path for
  // user code. This parameter is not used when the application runs in local mode.
  // Only searching the top level under a directory.
  std::vector<std::string> code_search_path;

  // The command line args to be appended as parameters of the `ray start` command. It
  // takes effect only if Ray head is started by a driver. Run `ray start --help` for
  // details.
  std::vector<std::string> head_args = {};

  // The default actor lifetime type, `DETACHED` or `NON_DETACHED`.
  ActorLifetime default_actor_lifetime = ActorLifetime::NON_DETACHED;

  // The job level runtime environments.
  boost::optional<RuntimeEnv> runtime_env;

  /* The following are unstable parameters and their use is discouraged. */

  // Prevents external clients without the password from connecting to Redis if provided.
  boost::optional<std::string> redis_password_;

  // A specific flag for internal `default_worker`. Please don't use it in user code.
  bool is_worker_ = false;

  // A namespace is a logical grouping of jobs and named actors.
  std::string ray_namespace = "";
};

}  // namespace ray
