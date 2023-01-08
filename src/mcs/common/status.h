//
// Created by lianyu on 2023/1/5.
//

#pragma once

#include <cstring>
#include <iosfwd>
#include <string>

namespace mcs {
  enum class StatusCode : char {
    OK = 0,
  };

  class Status {
  public:
    // Create a success status.
    Status() : state_(NULL) {}
    ~Status() { delete state_; }
  private:
    struct State {
      StatusCode code;
      std::string msg;
    };
    // OK status has a `NULL` state_.  Otherwise, `state_` points to
    // a `State` structure containing the error code and message(s)
    State *state_;
  };

} //namespace mcs
