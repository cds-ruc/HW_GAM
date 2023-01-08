//
// Created by hrh on 1/4/23.
//

#pragma once
#include <memory>
#include "msgpack.hpp"
#include <string_view>

#include "boost/optional.hpp"

namespace mcs {
  namespace internal {
    struct TaskArg {
      TaskArg() = default;
      TaskArg(TaskArg &&rhs) {
        buf = std::move(rhs.buf);
        id = rhs.id;
        meta_str = std::move(rhs.meta_str);
      }

      TaskArg(const TaskArg &) = delete;
      TaskArg &operator=(TaskArg const &) = delete;
      TaskArg &operator=(TaskArg &&) = delete;

      /// If the buf is initialized shows it is a value argument.
      boost::optional<msgpack::sbuffer> buf;
      /// If the id is initialized shows it is a reference argument.
      boost::optional<std::string> id;

      std::string_view meta_str;
    };

    using ArgsBuffer = msgpack::sbuffer;
    using ArgsBufferList = std::vector<ArgsBuffer>;

    using RemoteFunction = std::function<msgpack::sbuffer(const ArgsBufferList &)>;
    using RemoteFunctionMap_t = std::unordered_map<std::string, RemoteFunction>;

    using RemoteMemberFunction =
            std::function<msgpack::sbuffer(msgpack::sbuffer *, const ArgsBufferList &)>;
    using RemoteMemberFunctionMap_t = std::unordered_map<std::string, RemoteMemberFunction>;
  }  // namespace internal
}  // namespace mcs