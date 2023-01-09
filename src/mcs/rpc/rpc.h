//
// Created by lianyu on 2023/1/10.
//

#pragma once

namespace rpc {
  struct ObjectReference {
          // ObjectID that the worker has a reference to.
          int object_id = 1;
          // The address of the object's owner.
          int owner_address = 2;
          // Language call site of the object reference (i.e., file and line number).
          // Used to print debugging information if there is an error retrieving the
          // object.
          int call_site = 3;
  };

  struct TaskArg {
    int a;
  };
}
