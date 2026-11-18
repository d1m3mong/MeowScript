#pragma once

#include <optional>
#include <string_view>
#include <unordered_map>
enum class PrimitiveKind {
  I8,
  I16,
  I32,
  I64,
  U8,
  U16,
  U32,
  U64,
  F32,
  F64,

  Bool,
  String,
};

struct TypeRef {
  std::string_view text;
  std::optional<PrimitiveKind> primitive;
};

inline std::optional<PrimitiveKind> lookupPrimitive(std::string_view name) {
  static const std::unordered_map<std::string_view, PrimitiveKind> table = {
      {"i8", PrimitiveKind::I8},     {"i16", PrimitiveKind::I16},
      {"i32", PrimitiveKind::I32},   {"i64", PrimitiveKind::I64},
      {"u8", PrimitiveKind::U8},     {"u16", PrimitiveKind::U16},
      {"u32", PrimitiveKind::U32},   {"u64", PrimitiveKind::U64},
      {"f32", PrimitiveKind::F32},   {"f64", PrimitiveKind::F64},
      {"bool", PrimitiveKind::Bool}, {"string", PrimitiveKind::String},
  };

  auto it = table.find(name);
  if (it == table.end())
    return std::nullopt;
  return it->second;
}
