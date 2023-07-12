//
// Created by root on 1/11/23.
//

#pragma once


#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "absl/synchronization/mutex.h"
#include "mcs/rpc/protocol.h"
#include "mcs/api/common_types.h"
#include "mcs/core/buffer.h"
// #include "mcs/task/task_util.h"
// #include "mcs/common/function_descriptor.h"
// #include "mcs/util/grpc_util.h"
// #include "mcs/common/id.h"
// #include "mcs/runtime/task/scheduling_resources.h"
// #include "mcs/runtime/task/task_common.h"
//#include "mcs/util/container_util.h"

extern "C" {
#include "mcs/util/sha256/sha256.h"
}


namespace mcs {

    
    class TaskSpecification {
    public:
        TaskSpecification(uint64_t task_id,
                          const std::string func_name,
                          uint64_t num_returns,
                          std::vector<std::shared_ptr<LocalMemoryBuffer>> args) {
                task_id_ = task_id;
                func_name_ = func_name;
                num_returns_ = num_returns;
                args_ = args;
            }
        

        size_t NumArgs() const;

        std::string FuncName() const;

        const uint8_t *ArgData(size_t arg_index) const;

        size_t ArgDataSize(size_t arg_index) const;

        const std::vector<std::shared_ptr<LocalMemoryBuffer>> ArgList() { return args_; }

        void SetAddress(std::string address) { address_ = address; }
        const std::string Address() { return address_; }
        void SetPort(uint64_t port) { port_ = port; }
        const uint64_t Port() { return port_; }

        const uint64_t TaskID() { return task_id_; }
        // rpc::Regiser::TaskSpec build_message();

    private:
        uint64_t task_id_;                                      //任务ID
        //原始数据
        std::string func_name_;                                 //函数名
        uint64_t num_returns_;                                  //返回值个数
        std::vector<std::shared_ptr<LocalMemoryBuffer>> args_;  //参数
        
        std::string address_;                                   //执行地址
        uint64_t port_;                                         //端口

        //需要转换成通信的数据
        // rpc::Regiser::TaskSpec message_;
    };

}  // namespace mcs
