//
// Created by hrh on 1/4/23.
//
#pragma once
#include <string>

namespace mcs {

class McsConfig {
public:
  //主机地址
  std::string address = "";
  //端口
  int port = 0;
  //是否是worker
  bool is_worker_ = false;
};

}  // namespace mcs
