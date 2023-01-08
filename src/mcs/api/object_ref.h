//
// Created by lianyu on 2023/1/9.
//

#pragma once

#include <mcs/api/mcs_runtime_holder.h>
#include <mcs/api/serializer.h>
#include <mcs/api/type_traits.h>

#include <memory>
#include <msgpack.hpp>
#include <utility>

namespace mcs {

  template <typename T>
  class ObjectRef;

/// Common helper functions used by ObjectRef<T> and ObjectRef<void>;
  inline void CheckResult(const std::shared_ptr<msgpack::sbuffer> &packed_object) {
    bool has_error =
            mcs::internal::Serializer::HasError(packed_object->data(), packed_object->size());
    if (has_error) {
      auto tp = mcs::internal::Serializer::Deserialize<std::tuple<int, std::string>>(
              packed_object->data(), packed_object->size(), 1);
      std::string err_msg = std::get<1>(tp);
      throw mcs::internal::McsTaskException(err_msg);
    }
  }

  inline void CopyAndAddReference(std::string &dest_id, const std::string &id) {
    dest_id = id;
    mcs::internal::GetMcsRuntime()->AddLocalReference(id);
  }

  inline void SubReference(const std::string &id) {
    mcs::internal::GetMcsRuntime()->RemoveLocalReference(id);
  }

/// Represents an object in the object store..
/// \param T The type of object.
  template <typename T>
  class ObjectRef {
  public:
    ObjectRef();
    ~ObjectRef();
    // Used to identify its type.
    static bool IsObjectRef() { return true; }

    ObjectRef(ObjectRef &&rhs) {
      SubReference(rhs.id_);
      CopyAndAddReference(id_, rhs.id_);
      rhs.id_ = {};
    }

    ObjectRef &operator=(ObjectRef &&rhs) {
      if (rhs == *this) {
        return *this;
      }

      SubReference(id_);
      SubReference(rhs.id_);
      CopyAndAddReference(id_, rhs.id_);
      rhs.id_ = {};
      return *this;
    }

    ObjectRef(const ObjectRef &rhs) { CopyAndAddReference(id_, rhs.id_); }

    ObjectRef &operator=(const ObjectRef &rhs) {
      if (rhs == *this) {
        return *this;
      }

      SubReference(id_);
      CopyAndAddReference(id_, rhs.id_);
      return *this;
    }

    ObjectRef(const std::string &id);

    bool operator==(const ObjectRef<T> &object) const;

    /// Get a untyped ID of the object
    const std::string &ID() const;

    /// Get the object from the object store.
    /// This method will be blocked until the object is ready.
    ///
    /// \return shared pointer of the result.
    std::shared_ptr<T> Get() const;

    /// Make ObjectRef serializable
    MSGPACK_DEFINE(id_);

  private:
    std::string id_;
  };

// ---------- implementation ----------
  template <typename T>
  inline static std::shared_ptr<T> GetFromRuntime(const ObjectRef<T> &object) {
    auto packed_object = internal::GetMcsRuntime()->Get(object.ID());
    CheckResult(packed_object);

    if (mcs::internal::Serializer::IsXLang(packed_object->data(), packed_object->size())) {
      return mcs::internal::Serializer::Deserialize<std::shared_ptr<T>>(
              packed_object->data(), packed_object->size(), internal::XLANG_HEADER_LEN);
    }

    if constexpr (mcs::internal::is_actor_handle_v<T>) {
      auto actor_handle = mcs::internal::Serializer::Deserialize<std::string>(
              packed_object->data(), packed_object->size());
      return std::make_shared<T>(T::FromBytes(actor_handle));
    }

    return mcs::internal::Serializer::Deserialize<std::shared_ptr<T>>(
            packed_object->data(), packed_object->size());
  }

  template <typename T>
  ObjectRef<T>::ObjectRef() {}

  template <typename T>
  ObjectRef<T>::ObjectRef(const std::string &id) {
    CopyAndAddReference(id_, id);
  }

  template <typename T>
  ObjectRef<T>::~ObjectRef() {
    SubReference(id_);
  }

  template <typename T>
  inline bool ObjectRef<T>::operator==(const ObjectRef<T> &object) const {
    return id_ == object.id_;
  }

  template <typename T>
  const std::string &ObjectRef<T>::ID() const {
    return id_;
  }

  template <typename T>
  inline std::shared_ptr<T> ObjectRef<T>::Get() const {
    return GetFromRuntime(*this);
  }

  template <>
  class ObjectRef<void> {
  public:
    ObjectRef() = default;
    ~ObjectRef() { SubReference(id_); }
    // Used to identify its type.
    static bool IsObjectRef() { return true; }

    ObjectRef(const ObjectRef &rhs) { CopyAndAddReference(id_, rhs.id_); }

    ObjectRef &operator=(const ObjectRef &rhs) {
      CopyAndAddReference(id_, rhs.id_);
      return *this;
    }

    ObjectRef(const std::string &id) { CopyAndAddReference(id_, id); }

    bool operator==(const ObjectRef<void> &object) const { return id_ == object.id_; }

    /// Get a untyped ID of the object
    const std::string &ID() const { return id_; }

    /// Get the object from the object store.
    /// This method will be blocked until the object is ready.
    ///
    /// \return shared pointer of the result.
    void Get() const {
      auto packed_object = internal::GetMcsRuntime()->Get(id_);
      CheckResult(packed_object);
    }

    /// Make ObjectRef serializable
    MSGPACK_DEFINE(id_);

  private:
    std::string id_;
  };

}  // namespace mcs
