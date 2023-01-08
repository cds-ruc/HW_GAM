//
// Created by lianyu on 2023/1/9.
//

#pragma once

#include <mcs/api/object_ref.h>
#include <mcs/api/serializer.h>
#include <mcs/api/type_traits.h>
#include <mcs/api/xlang_function.h>

#include <msgpack.hpp>
#include <type_traits>

namespace mcs {
  namespace internal {

    class Arguments {
    public:
      template <typename OriginArgType, typename InputArgTypes>
      static void WrapArgsImpl(LangType lang_type,
                               std::vector<TaskArg> *task_args,
                               InputArgTypes &&arg) {
        if constexpr (is_object_ref_v<OriginArgType>) {
          PushReferenceArg(task_args, std::forward<InputArgTypes>(arg));
        } else if constexpr (is_object_ref_v<InputArgTypes>) {
          // core_worker submitting task callback will get the value of an ObjectRef arg, but
          // local mode we don't call core_worker submit task, so we need get the value of an
          // ObjectRef arg only for local mode.
          if (McsRuntimeHolder::Instance().Runtime()->IsLocalMode()) {
            auto buffer = McsRuntimeHolder::Instance().Runtime()->Get(arg.ID());
            PushValueArg(task_args, std::move(*buffer));
          } else {
            PushReferenceArg(task_args, std::forward<InputArgTypes>(arg));
          }
        } else {
          if (lang_type == LangType::CPP) {
            if constexpr (is_actor_handle_v<InputArgTypes>) {
              auto serialized_actor_handle =
                      McsRuntimeHolder::Instance().Runtime()->SerializeActorHandle(arg.ID());
              msgpack::sbuffer buffer = Serializer::Serialize(serialized_actor_handle);
              PushValueArg(task_args, std::move(buffer));
            } else {
              msgpack::sbuffer buffer =
                      Serializer::Serialize(std::forward<InputArgTypes>(arg));
              PushValueArg(task_args, std::move(buffer));
            }
          } else {
            // Fill dummy field for handling kwargs.
            if (lang_type == LangType::PYTHON) {
              msgpack::sbuffer dummy_buf(METADATA_STR_DUMMY.size());
              dummy_buf.write(METADATA_STR_DUMMY.data(), METADATA_STR_DUMMY.size());
              PushValueArg(task_args, std::move(dummy_buf), METADATA_STR_RAW);
            }
            // Below applies to both PYTHON and JAVA.
            auto data_buf = Serializer::Serialize(std::forward<InputArgTypes>(arg));
            auto len_buf = Serializer::Serialize(data_buf.size());

            msgpack::sbuffer buffer(XLANG_HEADER_LEN + data_buf.size());
            buffer.write(len_buf.data(), len_buf.size());
            for (size_t i = 0; i < XLANG_HEADER_LEN - len_buf.size(); ++i) {
              buffer.write("", 1);
            }
            buffer.write(data_buf.data(), data_buf.size());

            PushValueArg(task_args, std::move(buffer), METADATA_STR_XLANG);
          }
        }
      }

      template <typename OriginArgsTuple, size_t... I, typename... InputArgTypes>
      static void WrapArgs(LangType lang_type,
                           std::vector<TaskArg> *task_args,
                           std::index_sequence<I...>,
                           InputArgTypes &&...args) {
        (void)std::initializer_list<int>{
                (WrapArgsImpl<std::tuple_element_t<I, OriginArgsTuple>>(
                        lang_type, task_args, std::forward<InputArgTypes>(args)),
                        0)...};
        /// Silence gcc warning error.
        (void)task_args;
        (void)lang_type;
      }

    private:
      static void PushValueArg(std::vector<TaskArg> *task_args,
                               msgpack::sbuffer &&buffer,
                               std::string_view meta_str = "") {
        /// Pass by value.
        TaskArg task_arg;
        task_arg.buf = std::move(buffer);
        if (!meta_str.empty()) task_arg.meta_str = std::move(meta_str);
        task_args->emplace_back(std::move(task_arg));
      }

      template <typename TaskArg, typename T>
      static void PushReferenceArg(std::vector<TaskArg> *task_args, T &&arg) {
        /// Pass by reference.
        TaskArg task_arg{};
        task_arg.id = arg.ID();
        task_args->emplace_back(std::move(task_arg));
      }
    };

  }  // namespace internal
}  // namespace mcs
