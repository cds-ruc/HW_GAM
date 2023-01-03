//
// Created by hrh on 1/4/23.
//

#pragma once
#include <memory>
#include <string_view>

#include "msgpack.hpp"

namespace mcs {
  namespace internal {
    using ArgsBuffer = msgpack::sbuffer;
    using ArgsBufferList = std::vector<ArgsBuffer>;

    using RemoteFunction = std::function<msgpack::sbuffer(const ArgsBufferList &)>;
    using RemoteFunctionMap_t = std::unordered_map<std::string, RemoteFunction>;
  }  // namespace internal
}  // namespace mcs