//
// Created by lianyu on 2023/1/9.
//

#pragma once

#include <mcs/api/object_ref.h>

#include <boost/callable_traits.hpp>
#include <type_traits>

namespace mcs {
  namespace internal {

    template <typename T>
    struct FilterArgType {
      using type = T;
    };

    template <typename T>
    struct FilterArgType<ObjectRef<T>> {
    using type = T;
  };

  template <typename T>
  struct FilterArgType<ObjectRef<T> &> {
  using type = T;
};

template <typename T>
struct FilterArgType<ObjectRef<T> &&> {
using type = T;
};

template <typename T>
struct FilterArgType<const ObjectRef<T> &> {
  using type = T;
};

template <typename F, typename... Args>
struct is_invocable
        : std::is_constructible<
                std::function<void(Args...)>,
                std::reference_wrapper<typename std::remove_reference<F>::type>> {};

template <typename Function, typename... Args>
inline std::enable_if_t<!std::is_member_function_pointer<Function>::value> StaticCheck() {
  static_assert(is_invocable<Function, typename FilterArgType<Args>::type...>::value ||
                is_invocable<Function, Args...>::value,
                "arguments not match");
}

template <typename Function, typename... Args>
inline std::enable_if_t<std::is_member_function_pointer<Function>::value> StaticCheck() {
  using ActorType = boost::callable_traits::class_of_t<Function>;
  static_assert(
          is_invocable<Function, ActorType &, typename FilterArgType<Args>::type...>::value ||
          is_invocable<Function, ActorType &, Args...>::value,
          "arguments not match");
}

}  // namespace internal
}  // namespace mcs

