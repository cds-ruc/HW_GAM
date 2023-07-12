//
// Created by lianyu on 2023/1/9.
//

#include "local_mode_task_submitter.h"

#include <mcs/api/mcs_exception.h>

#include <boost/asio/post.hpp>
#include <memory>

#include <mcs/runtime/abstract_mcs_runtime.h>
#include <mcs/task/task_spec.h>
#include <mcs/rpc/client.h>
#include <mcs/core/config_internal.h>
#include <photon/photon.h>

namespace mcs {
  namespace internal {

    LocalModeTaskSubmitter::LocalModeTaskSubmitter(
            LocalModeMcsRuntime &local_mode_mcs_runtime)
            : local_mode_mcs_runtime_(local_mode_mcs_runtime) {
      thread_pool_.reset(new boost::asio::thread_pool(10));
    }

    //提交任务
    int64_t LocalModeTaskSubmitter::SubmitTask(InvocationSpec &invocation, const CallOptions &call_options) {
      uint64_t task_id = local_mode_mcs_runtime_.CreateNewTask();
      TaskSpecification task_specification(
                                task_id,
                                invocation.remote_function_holder.function_name,
                                1,
                                invocation.args);
      std::shared_ptr<absl::Mutex> mutex;
      AbstractMcsRuntime *runtime = &local_mode_mcs_runtime_;
      for(size_t i = 0; i < mcs::internal::ConfigInternal::Instance().resources.size(); i++) {
        if (mcs::internal::ConfigInternal::Instance().resources[i].core_num > 0) {
            NodeResource node = mcs::internal::ConfigInternal::Instance().resources[i];
            mcs::internal::ConfigInternal::Instance().resources[i].core_num--;
            task_specification.SetAddress(node.address);
            task_specification.SetPort(node.port);

            boost::asio::post(
              *thread_pool_.get(),
              std::bind(
                      [mutex, this](TaskSpecification &ts, AbstractMcsRuntime *rn) {
                        if (mutex) {
                          absl::MutexLock lock(mutex.get());
                        }
                        photon::init();
                        DEFER(photon::fini());
                        mcs::rpc::McsClient client;
                        photon::net::EndPoint ep = photon::net::EndPoint(photon::net::IPAddr(ts.Address().c_str()),
                                                  ts.Port());
                        client.rpc_resister(ep, ts.FuncName(), ts.ArgList());
                        rn->ChangeTaskStatus(ts.TaskID());
                      },
                      std::move(task_specification), runtime));
        }
      }
      return task_id;
    }


  }  // namespace internal
}  // namespace mcs