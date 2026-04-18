#pragma once

#if MEADOW_HAS_MAGIC_ENUM
  #include <magic_enum/magic_enum.hpp>
#endif

#include <optional>
#include <type_traits>
#include <utility>

#if MEADOW_HAS_MAGIC_ENUM
template<class E, class T>
std::optional<std::decay_t<E>> enum_cast_from_any_int(T t)
{
    using ut = std::underlying_type_t<E>;
    return std::in_range<ut>(t) ? magic_enum::enum_cast<E>(static_cast<ut>(t)) : std::nullopt;
}
#endif
