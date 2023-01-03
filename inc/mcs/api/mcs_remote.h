//
// Created by hrh on 1/4/23.
//

#pragma once

#include <mcs/api/function_manager.h>

namespace mcs {
  namespace internal {

    inline static std::vector<boost::string_view> GetFunctionNames(boost::string_view str) {
      std::vector<boost::string_view> output;
      size_t first = 0;

      while (first < str.size()) {
        auto second = str.find_first_of(",", first);

        if (first != second) {
          auto sub_str = str.substr(first, second - first);
          if (sub_str.find_first_of('(') != boost::string_view::npos) {
            second = str.find_first_of(")", first) + 1;
          }
          if (str[first] == ' ') {
            first++;
          }

          auto name = str.substr(first, second - first);
          if (name.back() == ' ') {
            name.remove_suffix(1);
          }
          output.emplace_back(name);
        }

        if (second == boost::string_view::npos) break;

        first = second + 1;
      }

      return output;
    }

    template <typename T, typename... U>
    inline static int RegisterRemoteFunctions(const T &t, U... u) {
      int index = 0;
      const auto func_names = GetFunctionNames(t);
      (void)std::initializer_list<int>{
          (FunctionManager::Instance().RegisterRemoteFunction(
              std::string(func_names[index].data(), func_names[index].length()), u),
              index++,
              0)...};
      return 0;
    }
#define CONCATENATE_DIRECT(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_DIRECT(s1, s2)
#define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __LINE__)
  }  // namespace internal

#define MCS_REMOTE(...)                 \
  inline auto ANONYMOUS_VARIABLE(var) = \
      mcs::internal::RegisterRemoteFunctions(#__VA_ARGS__, __VA_ARGS__);

#define MCS_FUNC(f, ...) mcs::internal::underload<__VA_ARGS__>(f)
} // namespace mcs
