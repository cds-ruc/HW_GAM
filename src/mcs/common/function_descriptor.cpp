//
// Created by root on 1/11/23.
//

#include "function_descriptor.h"

namespace mcs {
    FunctionDescriptor FunctionDescriptorBuilder::Empty() {
        static mcs::FunctionDescriptor empty =
                mcs::FunctionDescriptor(new EmptyFunctionDescriptor());
        return empty;
    }

    FunctionDescriptor FunctionDescriptorBuilder::BuildJava(const std::string &class_name,
                                                            const std::string &function_name,
                                                            const std::string &signature) {
        rpc::FunctionDescriptor descriptor;
        auto typed_descriptor = descriptor.mutable_java_function_descriptor();
        typed_descriptor->set_class_name(class_name);
        typed_descriptor->set_function_name(function_name);
        typed_descriptor->set_signature(signature);
        return mcs::FunctionDescriptor(new JavaFunctionDescriptor(std::move(descriptor)));
    }

    FunctionDescriptor FunctionDescriptorBuilder::BuildPython(
            const std::string &module_name,
            const std::string &class_name,
            const std::string &function_name,
            const std::string &function_hash) {
        rpc::FunctionDescriptor descriptor;
        auto typed_descriptor = descriptor.mutable_python_function_descriptor();
        typed_descriptor->set_module_name(module_name);
        typed_descriptor->set_class_name(class_name);
        typed_descriptor->set_function_name(function_name);
        typed_descriptor->set_function_hash(function_hash);
        return mcs::FunctionDescriptor(new PythonFunctionDescriptor(std::move(descriptor)));
    }

    FunctionDescriptor FunctionDescriptorBuilder::BuildCpp(const std::string &function_name,
                                                           const std::string &caller,
                                                           const std::string &class_name) {
        rpc::FunctionDescriptor descriptor;
        auto typed_descriptor = descriptor.mutable_cpp_function_descriptor();
        typed_descriptor->set_function_name(function_name);
        typed_descriptor->set_caller(caller);
        typed_descriptor->set_class_name(class_name);
        return mcs::FunctionDescriptor(new CppFunctionDescriptor(std::move(descriptor)));
    }

    FunctionDescriptor FunctionDescriptorBuilder::FromProto(rpc::FunctionDescriptor message) {
        switch (message.function_descriptor_case()) {
            case mcs::FunctionDescriptorType::kJavaFunctionDescriptor:
                return mcs::FunctionDescriptor(new mcs::JavaFunctionDescriptor(std::move(message)));
            case mcs::FunctionDescriptorType::kPythonFunctionDescriptor:
                return mcs::FunctionDescriptor(new mcs::PythonFunctionDescriptor(std::move(message)));
            case mcs::FunctionDescriptorType::kCppFunctionDescriptor:
                return mcs::FunctionDescriptor(new mcs::CppFunctionDescriptor(std::move(message)));
            default:
                break;
        }
        // When TaskSpecification() constructed without function_descriptor set,
        // we should return a valid mcs::FunctionDescriptor instance.
        return FunctionDescriptorBuilder::Empty();
    }

    FunctionDescriptor FunctionDescriptorBuilder::FromVector(
            rpc::Language language, const std::vector<std::string> &function_descriptor_list) {
        if (language == rpc::Language::JAVA) {
            MCS_CHECK(function_descriptor_list.size() == 3);
            return FunctionDescriptorBuilder::BuildJava(
                    function_descriptor_list[0],  // class name
                    function_descriptor_list[1],  // function name
                    function_descriptor_list[2]   // signature
            );
        } else if (language == rpc::Language::PYTHON) {
            MCS_CHECK(function_descriptor_list.size() == 4);
            return FunctionDescriptorBuilder::BuildPython(
                    function_descriptor_list[0],  // module name
                    function_descriptor_list[1],  // class name
                    function_descriptor_list[2],  // function name
                    function_descriptor_list[3]   // function hash
            );
        } else if (language == rpc::Language::CPP) {
            MCS_CHECK(function_descriptor_list.size() == 3);
            return FunctionDescriptorBuilder::BuildCpp(
                    function_descriptor_list[0],   // function name
                    function_descriptor_list[1],   // caller
                    function_descriptor_list[2]);  // class name
        } else {
            MCS_LOG(FATAL) << "Unspported language " << language;
            return FunctionDescriptorBuilder::Empty();
        }
    }

    FunctionDescriptor FunctionDescriptorBuilder::Deserialize(
            const std::string &serialized_binary) {
        rpc::FunctionDescriptor descriptor;
        descriptor.ParseFromString(serialized_binary);
        return FunctionDescriptorBuilder::FromProto(std::move(descriptor));
    }
}  // namespace mcs