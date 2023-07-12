#pragma once

#include <photon/rpc/serialize.h>

#include <cstdint>

namespace mcs {
  namespace rpc {
    /**
    * @brief interface means class with member deleivers `IID` as
    * interface.
    */
    struct Interface {
      const static uint32_t IID = 0x222;
    };


    // struct FunctionDescriptor {
    //   photon::rpc::string function_name;
    //   photon::rpc::string caller;
    //   photon::rpc::string class_name;
    // };





    struct Regiser : public Interface {
      const static uint32_t FID = 0x001;
      struct TaskArg {
        // Data for pass-by-value arguments.
        photon::rpc::array<uint8_t> data;
      };
      struct TaskSpec {
        photon::rpc::string function_name;
        photon::rpc::array<TaskArg> args;
        uint64_t num_returns;
      };
      struct Request : public photon::rpc::Message {
        photon::rpc::string function_name;
        photon::rpc::array<char> args_data;
        photon::rpc::array<char> args_size;
        PROCESS_FIELDS(function_name,args_data,args_size);
      };
      struct Response : public photon::rpc::Message {
        bool result;
        PROCESS_FIELDS(result);
      };
    };

    struct PutTask : public Interface {
      const static uint32_t FID = 0x002;

      struct Request : public photon::rpc::Message {
        size_t core_num;
        photon::rpc::string address;
        PROCESS_FIELDS(core_num, address);
      };
      struct Response : public photon::rpc::Message {
        bool result;
        PROCESS_FIELDS(result);
      };
    };

    struct RunFunc : public Interface {
      const static uint32_t FID = 0x003;
      struct Request : public photon::rpc::Message {
        size_t core_num;
        photon::rpc::string address;
        PROCESS_FIELDS(core_num, address);
      };
      struct Response : public photon::rpc::Message {
        bool result;
        PROCESS_FIELDS(result);
      };
    };

  }
}

