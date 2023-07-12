//
// Created by root on 1/11/23.
//

#include "task_spec.h"

#include <boost/functional/hash.hpp>
#include <sstream>

#include "mcs/util/logging.h"

namespace mcs {


    size_t TaskSpecification::NumArgs() const { return args_.size(); }

    std::string TaskSpecification::FuncName() const { 
        return func_name_;
    }

    const uint8_t *TaskSpecification::ArgData(size_t arg_index) const {
        return reinterpret_cast<const uint8_t *>(args_[arg_index]->Data());
    }

    size_t TaskSpecification::ArgDataSize(size_t arg_index) const {
        return args_[arg_index]->Size();
    }

    // rpc::Regiser::TaskSpec TaskSpecification::build_message() {
    //     message_.function_name.assign(func_name_);
    //     message_.num_returns = num_returns_;
    //     std::vector<rpc::Regiser::TaskArg> args;
    //     for (size_t i = 0 ; i < args_.size(); i++) {
    //         rpc::Regiser::TaskArg arg;
    //         arg.data.assign(args_[i]->Data(), args_[i]->Size());
    //         args.push_back(arg);
    //     }
    //     message_.args.assign(args);
    //     printf("write args %d size = %d\n", 0, message_.args[0].data.size());
    //     return message_;
    // }

}  // namespace mcs 