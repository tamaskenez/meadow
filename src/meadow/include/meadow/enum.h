#pragma once

#if MEADOW_HAS_MAGIC_ENUM
  #include <magic_enum/magic_enum.hpp>
  #include <magic_enum/magic_enum_containers.hpp>
#endif

#include <optional>
#include <type_traits>
#include <utility>

#if MEADOW_HAS_MAGIC_ENUM
template<class E, class T>
    requires std::integral<T>
std::optional<std::decay_t<E>> enum_cast_from_any_int(T t)
{
    using ut = std::underlying_type_t<E>;
    return std::in_range<ut>(t) ? magic_enum::enum_cast<E>(static_cast<ut>(t)) : std::nullopt;
}

template<class E, class T>
    requires std::floating_point<T>
std::optional<std::decay_t<E>> enum_cast_from_round_float(T t)
{
    using ut = std::underlying_type_t<E>;
    if (auto e = try_iround<ut>(t)) {
        return magic_enum::enum_cast<E>(*e);
    } else {
        return std::nullopt;
    }
}

// class Flags allows enums to be used as flags.
// E must be a scoped enum.
//
// The explicit underlying values assigned to the enum values are ignored. No need to assign powers-of-2.
// Moreover, assigning underlying values above MAGIC_ENUM_RANGE_MAX (default: 127) will break the operations since
// magic_enum can't index values above that.
template<class E>
    requires magic_enum::is_scoped_enum_v<E>
class Flags
{
public:
    constexpr Flags() = default;
    constexpr Flags(E e)
        : bitset({e})
    {
    }
    constexpr bool operator&(E e) const
    {
        return bitset.test(e);
    }
    constexpr auto operator|(E e) const
    {
        auto result = *this;
        result.bitset.set(e);
        return result;
    }
    constexpr bool any() const
    {
        return bitset.any();
    }
    constexpr bool all() const
    {
        return bitset.all();
    }

private:
    magic_enum::containers::bitset<E> bitset;
};

template<class E>
    requires magic_enum::is_scoped_enum_v<E>
constexpr Flags<E> operator|(E x, E y)
{
    return Flags<E>(x) | y;
}
#endif
