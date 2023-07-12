
#include "client.h"

#include <photon/common/alog-stdstring.h>
#include <photon/common/alog.h>
#include <photon/common/iovector.h>
#include <photon/net/socket.h>

namespace mcs {
  namespace rpc {
    void McsClient::rpc_resister(photon::net::EndPoint ep, std::string function_name, std::vector<std::shared_ptr<LocalMemoryBuffer>> args) {
      Regiser::Request req;
      req.function_name.assign(function_name);
      std::vector<char> args_data;
      std::vector<char> args_size;
      for (size_t i = 0 ; i < args.size(); i++) {
          //一个一个字节加
          for (size_t j = 0; j < args[i]->Size(); j++) {
            args_data.push_back(*(args[i]->Data() + j));
          }
          args_size.push_back(args[i]->Size());
      }
      req.args_data = args_data;
      req.args_size = args_size;
      printf("req arg value %d size = %d\n", (char)req.args_data[0], req.args_size[0]);
      Regiser::Response resp;
      int ret = 0;
      auto stub = pool->get_stub(ep, false);
      // if (!stub) return -1;
      DEFER(pool->put_stub(ep, ret < 0));
      ret = stub->call<Regiser>(req, resp);
    }

    void McsClient::rpc_put_task(photon::net::EndPoint ep) {
      PutTask::Request req;
      req.address = "127.0.0.1";
      req.core_num = 10;
      // req.task_spec = task_spec;
      PutTask::Response resp;
      int ret = 0;
      auto stub = pool->get_stub(ep, false);
      // if (!stub) return -1;
      DEFER(pool->put_stub(ep, ret < 0));
      ret = stub->call<PutTask>(req, resp);
    }

    void McsClient::rpc_run_func(photon::net::EndPoint ep) {
      RunFunc::Request req;
      RunFunc::Response resp;
      int ret = 0;
      auto stub = pool->get_stub(ep, false);
      // if (!stub) return -1;
      DEFER(pool->put_stub(ep, ret < 0));
      ret = stub->call<RunFunc>(req, resp);
    }
  }
}