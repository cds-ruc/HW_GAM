#pragma once

#include <photon/rpc/rpc.h>
#include <sys/uio.h>
#include "mcs/core/buffer.h"
#include "protocol.h"
namespace mcs {
  namespace rpc {
    struct McsClient {
        std::unique_ptr<photon::rpc::StubPool> pool;

        // create a tcp rpc connection pool
        // unused connections will be drop after 10 seconds(10UL*1000*1000)
        // TCP connection will failed in 1 second(1UL*1000*1000) if not accepted
        // and connection send/recv will take 5 socneds(5UL*1000*1000) as timedout
        McsClient()
            : pool(photon::rpc::new_stub_pool(10UL * 1000 * 1000, 1UL * 1000 * 1000,
                                              5UL * 1000 * 1000)) {}

        void rpc_resister(photon::net::EndPoint ep, std::string function_name, std::vector<std::shared_ptr<LocalMemoryBuffer>> args);

        void rpc_put_task(photon::net::EndPoint ep);

        void rpc_run_func(photon::net::EndPoint ep);
    };
  }
}