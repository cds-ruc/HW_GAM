//
// Created by lianyu on 2023/1/10.
//

#include "mcs/common/mcs_object.h"

#include "msgpack.hpp"

namespace {
  std::shared_ptr<mcs::LocalMemoryBuffer> MakeBufferFromString(const uint8_t *data,
                                                               size_t data_size) {
    auto metadata = const_cast<uint8_t *>(data);
    auto meta_buffer =
            std::make_shared<mcs::LocalMemoryBuffer>(metadata, data_size, /*copy_data=*/true);
    return meta_buffer;
  }

  std::shared_ptr<mcs::LocalMemoryBuffer> MakeBufferFromString(const std::string &str) {
    return MakeBufferFromString(reinterpret_cast<const uint8_t *>(str.data()), str.size());
  }

//  std::shared_ptr<mcs::LocalMemoryBuffer> MakeErrorMetadataBuffer(
//          mcs::rpc::ErrorType error_type) {
//    std::string meta = std::to_string(static_cast<int>(error_type));
//    return MakeBufferFromString(meta);
//  }

/// Serialize the protobuf message to msg pack.
///
/// Mcs uses Msgpack for cross-language object serialization.
/// This method creates a msgpack serialized buffer that contains
/// serialized protobuf message.
///
/// Language frontend can deseiralize this object to obtain
/// data stored in a given protobuf. Check `serialization.py` to see
/// how this works.
///
/// NOTE: The function guarantees that the returned buffer contains data.
///
/// \param protobuf_message The protobuf message to serialize.
/// \return The buffer that contains serialized msgpack message.
//  template <class ProtobufMessage>
//  std::shared_ptr<mcs::LocalMemoryBuffer> MakeSerializedErrorBuffer(
//          const ProtobufMessage &protobuf_message) {
    // Structure of bytes stored in object store:

    // First serialize McsException by the following steps:
    // PB's McsException
    // --(PB Serialization)-->
    // --(msgpack Serialization)-->
    // msgpack_serialized_exception(MSE)

    // Then add it's length to the head(for coross-language deserialization):
    // [MSE's length(9 bytes)] [MSE]

//    std::string pb_serialized_exception;
//    protobuf_message.SerializeToString(&pb_serialized_exception);
//    msgpack::sbuffer msgpack_serialized_exception;
//    msgpack::packer<msgpack::sbuffer> packer(msgpack_serialized_exception);
//    packer.pack_bin(pb_serialized_exception.size());
//    packer.pack_bin_body(pb_serialized_exception.data(), pb_serialized_exception.size());
//    std::unique_ptr<mcs::LocalMemoryBuffer> final_buffer =
//            std::make_unique<mcs::LocalMemoryBuffer>(msgpack_serialized_exception.size() +
//                                                     kMessagePackOffset);
//    // copy msgpack-serialized bytes
//    std::memcpy(final_buffer->Data() + kMessagePackOffset,
//                msgpack_serialized_exception.data(),
//                msgpack_serialized_exception.size());
//    // copy offset
//    msgpack::sbuffer msgpack_int;
//    msgpack::pack(msgpack_int, msgpack_serialized_exception.size());
//    std::memcpy(final_buffer->Data(), msgpack_int.data(), msgpack_int.size());
//    MCS_CHECK(final_buffer->Data() != nullptr);
//    MCS_CHECK(final_buffer->Size() != 0);
//
//    return final_buffer;
//  }

}  // namespace

namespace mcs {

//  McsObject::McsObject(rpc::ErrorType error_type, const rpc::McsErrorInfo *mcs_error_info) {
//    if (mcs_error_info == nullptr) {
//      Init(nullptr, MakeErrorMetadataBuffer(error_type), {});
//      return;
//    }
//
//    const auto error_buffer = MakeSerializedErrorBuffer<rpc::McsErrorInfo>(*mcs_error_info);
//    Init(std::move(error_buffer), MakeErrorMetadataBuffer(error_type), {});
//    return;
//  }

//  bool McsObject::IsException(rpc::ErrorType *error_type) const {
//    if (metadata_ == nullptr) {
//      return false;
//    }
//    // TODO (kfstorm): metadata should be structured.
//    const std::string metadata(reinterpret_cast<const char *>(metadata_->Data()),
//                               metadata_->Size());
//    const auto error_type_descriptor = mcs::rpc::ErrorType_descriptor();
//    for (int i = 0; i < error_type_descriptor->value_count(); i++) {
//      const auto error_type_number = error_type_descriptor->value(i)->number();
//      if (metadata == std::to_string(error_type_number)) {
//        if (error_type) {
//          *error_type = rpc::ErrorType(error_type_number);
//        }
//        return true;
//      }
//    }
//    return false;
//  }

//  bool McsObject::IsInPlasmaError() const {
//    if (metadata_ == nullptr) {
//      return false;
//    }
//    const std::string metadata(reinterpret_cast<const char *>(metadata_->Data()),
//                               metadata_->Size());
//    return metadata == std::to_string(mcs::rpc::ErrorType::OBJECT_IN_PLASMA);
//  }

}  // namespace mcs