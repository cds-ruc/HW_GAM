//
// Created by hrh on 1/4/23.
//
#include <iostream>
#include <gflags/gflags.h>
#include <photon/common/utility.h>
#include <photon/io/signal.h>
#include <photon/photon.h>
#include "mcs/api.h"
#include "mcs/rpc/server.h"
#include "mcs/rpc/client.h"

void plus(int a, int b) {
  // printf("run plus\n");
  std::cout << "plus " << a << " + " << b << " = " <<  a + b << std::endl;
}
MCS_REMOTE(plus);

// int main() {
//   mcs::Init();
//   auto task_object = mcs::Task(plus).Remote(1, 2);

//   // sleep(1);
//   // int task_result = *(mcs::Get(task_object));
//   // std::cout << "task_result = " << task_result << std::endl;

//   return 0;
// }

DEFINE_string(workType, "worker", "有master/wroker两种, 默认工作类型是work");
DEFINE_int32(port, 33068, "Server 的监听端口, 0(默认) 随机端口");
DEFINE_string(host, "127.0.0.1", "server ip");

std::unique_ptr<mcs::rpc::McsServer> rpcservice;
static photon::net::EndPoint ep;

void handle_null(int) {}
void handle_term(int) { rpcservice.reset(); }

int main(int argc, char** argv) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    photon::init();
    DEFER(photon::fini());
    photon::sync_signal(SIGPIPE, &handle_null);
    photon::sync_signal(SIGTERM, &handle_term);
    photon::sync_signal(SIGINT, &handle_term);

    if (FLAGS_workType.c_str()[0] != 'w') {

      // start server
      // construct rpcservice
      rpcservice.reset(new mcs::rpc::McsServer());
      rpcservice->run(FLAGS_port);
    } else {
      // mcs::rpc::McsClient client;
      // ep = photon::net::EndPoint(photon::net::IPAddr(FLAGS_host.c_str()),
      //                           FLAGS_port);
      // client.rpc_resister(ep);
      mcs::Init();
      auto task_result = mcs::Task(plus).Remote(10010240,2412424);
      mcs::Get(task_result);

    }
    return 0;
}