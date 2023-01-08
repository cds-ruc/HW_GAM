//
// Created by lianyu on 2023/1/6.
//

#include "function_helper.h"

#include <boost/range/iterator_range.hpp>
#include <memory>

#include "logging.h"

namespace mcs {
  namespace internal {

    void FunctionHelper::LoadDll(const std::filesystem::path &lib_path) {
      MCS_LOG(INFO) << "Start loading the library " << lib_path << ".";

      auto it = libraries_.find(lib_path.string());
      if (it != libraries_.end()) {
        return;
      }

      MCS_CHECK(std::filesystem::exists(lib_path))
              << lib_path << " dynamic library not found.";

      std::shared_ptr<boost::dll::shared_library> lib = nullptr;
      try {
        lib = std::make_shared<boost::dll::shared_library>(
                lib_path.string(), boost::dll::load_mode::type::rtld_lazy);
      } catch (std::exception &e) {
        MCS_LOG(FATAL) << "Failed to load library, lib_path: " << lib_path
                       << ", failed reason: " << e.what();
        return;
      } catch (...) {
        MCS_LOG(FATAL) << "Failed to load library, lib_path: " << lib_path
                       << ", unknown failed reason.";
        return;
      }

      MCS_CHECK(libraries_.emplace(lib_path.string(), lib).second);

      try {
        auto entry_func = boost::dll::import_alias<msgpack::sbuffer(
        const std::string &, const ArgsBufferList &, msgpack::sbuffer *)>(
                *lib, "TaskExecutionHandler");
        auto function_names = LoadAllRemoteFunctions(lib_path.string(), *lib, entry_func);
        if (function_names.empty()) {
          MCS_LOG(WARNING)
                  << "No remote functions in library " << lib_path
                  << ". If you've already used Mcs::Task or Mcs::Actor in the library, please "
                     "ensure the remote functions have been registered by `MCS_REMOTE` macro.";
          lib->unload();
          return;
        }
        MCS_LOG(INFO) << "The library " << lib_path
                      << " is loaded successfully. The remote functions: " << function_names
                      << ".";
        return;
      } catch (boost::system::system_error &e) {
        MCS_LOG(INFO) << "The library " << lib_path << " isn't integrated with Mcs, skip it.";
        lib->unload();
      } catch (std::exception &e) {
        MCS_LOG(WARNING) << "Failed to get entry function from library: " << lib_path
                         << ", failed reason: " << e.what();
        lib->unload();
      } catch (...) {
        MCS_LOG(WARNING) << "Failed to get entry function from library: " << lib_path
                         << ", unknown failed reason.";
        lib->unload();
      }
      return;
    }

    std::string FunctionHelper::LoadAllRemoteFunctions(const std::string lib_path,
                                                       const boost::dll::shared_library &lib,
                                                       const EntryFuntion &entry_function) {
      static const std::string internal_function_name = "GetRemoteFunctions";
      if (!lib.has(internal_function_name)) {
        MCS_LOG(WARNING) << "Internal function '" << internal_function_name
                         << "' not found in " << lib_path;
        return "";
      }
      // Both default worker and user dynamic library static link libmcs_api.so which has a
      // singleton class McsRuntimeHolder, the user dynamic library will get a new un-init
      // instance of McsRuntimeHolder, so we need to init the McsRuntimeHolder singleton when
      // loading the user dynamic library to make sure the new instance valid.
      auto init_func =
              boost::dll::import_alias<void(std::shared_ptr<McsRuntime>)>(lib, "InitMcsRuntime");
      init_func(McsRuntimeHolder::Instance().Runtime());

      auto get_remote_func = boost::dll::import_alias<
              std::pair<const RemoteFunctionMap_t &, const RemoteMemberFunctionMap_t &>()>(
              lib, internal_function_name);
      std::string names_str;
      auto function_maps = get_remote_func();
      for (const auto &pair : function_maps.first) {
        names_str.append(pair.first).append(", ");
        remote_funcs_.emplace(pair.first, entry_function);
      }
      for (const auto &pair : function_maps.second) {
        names_str.append(pair.first).append(", ");
        remote_member_funcs_.emplace(pair.first, entry_function);
      }
      if (!names_str.empty()) {
        names_str.pop_back();
        names_str.pop_back();
      }
      return names_str;
    }

    void FindDynamicLibrary(std::filesystem::path path,
                            std::list<std::filesystem::path> &dynamic_libraries) {
#if __APPLE__
      static const std::unordered_set<std::string> dynamic_library_extension = {".dylib",
                                                                            ".so"};
#else
      static const std::unordered_set<std::string> dynamic_library_extension = {".so"};
#endif
      auto extension = path.extension();
      if (dynamic_library_extension.find(extension.string()) !=
          dynamic_library_extension.end()) {
        dynamic_libraries.emplace_back(path);
      }
    }

    void FunctionHelper::LoadFunctionsFromPaths(const std::vector<std::string> &paths) {
      std::list<std::filesystem::path> dynamic_libraries;
      // Lookup dynamic libraries from paths.
      for (auto path : paths) {
        if (std::filesystem::is_directory(path)) {
          for (auto &entry :
                  boost::make_iterator_range(std::filesystem::directory_iterator(path), {})) {
            FindDynamicLibrary(entry, dynamic_libraries);
          }
        } else if (std::filesystem::exists(path)) {
          FindDynamicLibrary(path, dynamic_libraries);
        } else {
          MCS_LOG(FATAL) << path << " dynamic library not found.";
        }
      }

      // Try to load all found libraries.
      for (auto lib : dynamic_libraries) {
        LoadDll(lib);
      }
    }

    const EntryFuntion &FunctionHelper::GetExecutableFunctions(
            const std::string &function_name) {
      auto it = remote_funcs_.find(function_name);
      if (it == remote_funcs_.end()) {
        throw McsFunctionNotFound("Executable function not found, the function name " +
                                  function_name);
      } else {
        return it->second;
      }
    }

    const EntryFuntion &FunctionHelper::GetExecutableMemberFunctions(
            const std::string &function_name) {
      auto it = remote_member_funcs_.find(function_name);
      if (it == remote_member_funcs_.end()) {
        throw McsFunctionNotFound("Executable member function not found, the function name " +
                                  function_name);
      } else {
        return it->second;
      }
    }

  }  // namespace internal
}  // namespace mcs