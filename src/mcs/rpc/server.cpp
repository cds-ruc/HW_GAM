#include "server.h"


#include <fcntl.h>
#include <photon/common/alog-stdstring.h>
#include <photon/common/alog.h>
#include <sys/uio.h>
#include <unistd.h>

#include <mcs/api/common_types.h>
#include "mcs/core/config_internal.h"
#include "mcs/task/task_executor.h"

namespace mcs {
  namespace rpc {
    int McsServer::do_rpc_service(Regiser::Request* req, Regiser::Response* resp,
      IOVector* iov, IStream*) {
        // LOG_INFO("receive node register ip = ` core_num = `", req->address.c_str(), req->core_num);
        // LOG_INFO("master has resoures count `", mcs::internal::ConfigInternal::Instance().resources.size());
        pool->call([&] {
          printf( "accept task : \n" );
          printf("function_name : %s\n" , req->function_name);
          // printf("args count : %d\n" , req->args.size());
          mcs::internal::ArgsBufferList args_buffer;
          for (size_t i = 0; i < req->args_size.size(); i++) {
              msgpack::sbuffer sbuf;
              printf("args value %d size = %d\n", (char)req->args_data[0], req->args_size[0]);
              if (i == 0) {
                sbuf.write((const char *)req->args_data.addr(), req->args_size[i]);
              } else {
                sbuf.write((const char *)req->args_data.addr() + req->args_size[i - 1], req->args_size[i]);
              }
              args_buffer.push_back(std::move(sbuf));
          }
          mcs::internal::TaskExecutionHandler(std::string(req->function_name.c_str()), args_buffer, nullptr);
        });
        return 0;
    }
    int McsServer::do_rpc_service(RunFunc::Request* req, RunFunc::Response* resp,
        IOVector* iov, IStream*) {

          return 0;
    }
    int McsServer::do_rpc_service(PutTask::Request* req, PutTask::Response* resp,
        IOVector* iov, IStream*) {
        printf("accept\n");
        // printf("accept task : \nfunction_name = %s \nreturn_num = %d\n", req->task_spec.function_name.c_str(), req->task_spec.num_returns);
        return 0;
    }      
    int McsServer::run(int port) {
      if (server->bind(port) < 0)
          LOG_ERRNO_RETURN(0, -1, "Failed to bind port `", port)
      if (server->listen() < 0) LOG_ERRNO_RETURN(0, -1, "Failed to listen");
      server->set_handler({this, &McsServer::serve});
      LOG_INFO("Started rpc server at `", server->getsockname());
      return server->start_loop(true);
    }
  }
}