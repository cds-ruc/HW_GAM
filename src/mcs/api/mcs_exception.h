//
// Created by lianyu on 2023/1/5.
//

#pragma once

#include <exception>
#include <string>

namespace mcs {
  namespace internal {

    class McsException : public std::exception {
    public:
      McsException(const std::string &msg) : msg_(msg) {};

      const char *what() const

      noexcept override{return msg_.c_str();};

      std::string msg_;
    };

    class McsActorException : public McsException {
    public:
      McsActorException(const std::string &msg) : McsException(msg){};
    };

    class McsTaskException : public McsException {
    public:
      McsTaskException(const std::string &msg) : McsException(msg){};
    };

    class McsWorkerException : public McsException {
    public:
      McsWorkerException(const std::string &msg) : McsException(msg){};
    };

    class UnreconstructableException : public McsException {
    public:
      UnreconstructableException(const std::string &msg) : McsException(msg){};
    };

    class McsFunctionNotFound : public McsException {
    public:
      McsFunctionNotFound(const std::string &msg) : McsException(msg){};
    };

    class McsRuntimeEnvException : public McsException {
    public:
      McsRuntimeEnvException(const std::string &msg) : McsException(msg){};
    };
  } //namespace internal
} //namespace mcs
