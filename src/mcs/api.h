//
// Created by hrh on 1/4/23.
//

#pragma once

#include <mcs/api/function_manager.h>
#include <mcs/api/task_caller.h>
#include "mcs/api/mcs_remote.h"
#include "mcs_config.h"
#include <mcs/util/logging.h>

namespace mcs {
  /// 使用参数
  void Init(mcs::McsConfig &config, int argc, char **argv);

  /// 使用默认配置初始化.
  void Init();

  /// 检查是否初始化
  bool IsInitialized();

  /// 关闭
  void Shutdown();

  /// 创建一个回调任务用来执行远程函数
/// 仅用于无状态的普通函数，例如 mcs::Task(Plus1).Remote(1),
/// mcs::Task(Plus).Remote(1, 2).
/// \param[in] func 被执行的分布式函数.
/// \return TaskCaller.
  template <typename F>
  mcs::internal::TaskCaller<F> Task(F func);


  /// Normal task.
  template <typename F>
  inline mcs::internal::TaskCaller<F> Task(F func) {
    static_assert(!mcs::internal::is_python_v<F>, "Must be a cpp function.");
    static_assert(!std::is_member_function_pointer_v<F>,
                  "Incompatible type: member function cannot be called with mcs::Task.");
    auto func_name = internal::FunctionManager::Instance().GetFunctionName(func);
    mcs::internal::RemoteFunctionHolder remote_func_holder(std::move(func_name));
    return mcs::internal::TaskCaller<F>(mcs::internal::GetMcsRuntime().get(),
                                        std::move(remote_func_holder));
  }
} // namespace mcs;