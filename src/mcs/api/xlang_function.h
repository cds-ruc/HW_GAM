//
// Created by lianyu on 2023/1/5.
//

#pragma once

#include <string>
#include <string_view>

namespace mcs {

  template <typename R>
  struct PyFunction {
    bool IsPython() { return true; }
    R operator()() { return {}; }

    std::string module_name;
    std::string function_name;
  };

  struct PyActorClass {
    bool IsPython() { return true; }
    void operator()() {}

    std::string module_name;
    std::string class_name;
    std::string function_name = "__init__";
  };

  template <typename R>
  struct PyActorMethod {
    bool IsPython() { return true; }
    R operator()() { return {}; }

    std::string function_name;
  };

  struct JavaActorClass {
    bool IsJava() { return true; }
    void operator()() {}
    std::string class_name;
    std::string module_name = "";
    std::string function_name = "<init>";
  };
  template <typename R>
  struct JavaActorMethod {
    bool IsJava() { return true; }
    R operator()() { return {}; }
    std::string function_name;
  };

  template <typename R>
  struct JavaFunction {
    bool IsJava() { return true; }
    R operator()() { return {}; }
    std::string class_name;
    std::string function_name;
  };

  namespace internal {

    enum class LangType {
      CPP,
      PYTHON,
      JAVA,
    };

    inline constexpr size_t XLANG_HEADER_LEN = 9;
    inline constexpr std::string_view METADATA_STR_DUMMY = "__MCS_DUMMY__";
    inline constexpr std::string_view METADATA_STR_RAW = "RAW";
    inline constexpr std::string_view METADATA_STR_XLANG = "XLANG";

  }  // namespace internal

}  // namespace mcs
