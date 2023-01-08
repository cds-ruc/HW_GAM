//
// Created by lianyu on 2023/1/5.
//

#pragma once

#include <string>
#include <vector>
#include "mcs/util/logging.h"
#include "absl/container/flat_hash_map.h"
#include "absl/random/random.h"

/// IP address by which the local node can be reached *from* the `address`.
///
/// The behavior should be the same as `node_ip_address_from_perspective` from Ray Python
/// code. See
/// https://stackoverflow.com/questions/2674314/get-local-ip-address-using-boost-asio.
///
/// TODO(kfstorm): Make this function shared code and migrate Python & Java to use this
/// function.
///
/// \param address The IP address and port of any known live service on the network
/// you care about.
/// \return The IP address by which the local node can be reached from the address.
std::string GetNodeIpAddress(const std::string &address = "8.8.8.8:53");

/// A helper function to combine command-line arguments in a platform-compatible manner.
/// The result of this function is intended to be suitable for the shell used by popen().
///
/// \param cmdline The command-line arguments to combine.
///
/// \return The command-line string, including any necessary escape sequences.
std::string CreateCommandLine(const std::vector<std::string> &args);

template <typename T>
void FillRandom(T *data) {
  MCS_CHECK(data != nullptr);

  thread_local absl::BitGen generator;
  for (size_t i = 0; i < data->size(); i++) {
    (*data)[i] = static_cast<uint8_t>(
            absl::Uniform(generator, 0, std::numeric_limits<uint8_t>::max()));
  }
}