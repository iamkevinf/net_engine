// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: message_type.proto

#ifndef PROTOBUF_message_5ftype_2eproto__INCLUDED
#define PROTOBUF_message_5ftype_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3005001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)

namespace protobuf_message_5ftype_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[1];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
inline void InitDefaults() {
}
}  // namespace protobuf_message_5ftype_2eproto

enum EMessageType {
  ENone = 0,
  ESCFrameInit = 100,
  ECSFrame = 101,
  ESCFrame = 102,
  ECSLogin = 10001,
  ESCLogin = 10002,
  ECSReady = 11001,
  ESCReady = 11002,
  EMax = 999999
};
bool EMessageType_IsValid(int value);
const EMessageType EMessageType_MIN = ENone;
const EMessageType EMessageType_MAX = EMax;
const int EMessageType_ARRAYSIZE = EMessageType_MAX + 1;

const ::google::protobuf::EnumDescriptor* EMessageType_descriptor();
inline const ::std::string& EMessageType_Name(EMessageType value) {
  return ::google::protobuf::internal::NameOfEnum(
    EMessageType_descriptor(), value);
}
inline bool EMessageType_Parse(
    const ::std::string& name, EMessageType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<EMessageType>(
    EMessageType_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)


namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::EMessageType> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::EMessageType>() {
  return ::EMessageType_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_message_5ftype_2eproto__INCLUDED
