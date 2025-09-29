#pragma once

#if MEADOW_HAS_ABSL == 1
  #include <absl/cleanup/cleanup.h>
  #include <absl/log/check.h>
  #include <absl/log/log.h>
#endif

#include <algorithm>
#include <any>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <expected>
#include <filesystem>
#include <format>
#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <numbers>
#include <optional>
#include <print>
#include <random>
#include <ranges>
#include <set>
#include <source_location>
#include <span>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

namespace chr = std::chrono;
namespace fs = std::filesystem;
namespace fs = std::filesystem;
namespace num = std::numbers;
namespace ra = std::ranges;
namespace this_thread = std::this_thread;
namespace vi = std::ranges::views;

#ifndef MEADOW_NO_USING_STD
using std::any;
using std::array;
using std::byte;
using std::clamp;
using std::cmp_equal;
using std::cmp_less;
using std::cmp_less_equal;
using std::cmp_not_equal;
using std::default_random_engine;
using std::deque;
using std::expected;
using std::format;
using std::function;
using std::holds_alternative;
using std::initializer_list;
using std::make_shared;
using std::make_unique;
using std::monostate;
using std::nullopt;
using std::optional;
using std::pair;
using std::print;
using std::println;
using std::shared_ptr;
using std::span;
using std::string;
using std::string_view;
using std::to_underlying;
using std::tuple;
using std::unexpected;
using std::uniform_int_distribution;
using std::uniform_real_distribution;
using std::unique_ptr;
using std::unordered_map;
using std::unordered_set;
using std::variant;
using std::vector;
using std::weak_ptr;
#endif

#define BEGIN_END(X) std::begin(X), std::end(X)
#define MOVE(X) std::move(X)

inline void __inline_void_function_with_empty_body__() {}
#define NOP __inline_void_function_with_empty_body__()

#define UNUSED [[maybe_unused]]

#define PP_CONCAT2(A, B) A##B
#define PP_CONCAT(A, B) PP_CONCAT2(A, B)

#define HOLDS(VARIANT, ALTERNATIVE) std::holds_alternative<ALTERNATIVE>(VARIANT)

#if MEADOW_HAS_ABSL == 0
[[noreturn]] void meadow_check_failed_handler(
  const char* condition, const std::source_location location = std::source_location::current()
);
  #ifdef NDEBUG
    #define CHECK(COND)                             \
        do {                                        \
            if (!(COND)) {                          \
                meadow_check_failed_handler(#COND); \
            }                                       \
        } while (0)
  #else
    #define CHECK(COND) assert(COND)
  #endif
#endif

// switch_variant from https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template<typename... Ts, typename Variant>
auto switch_variant(Variant&& variant, Ts&&... ts)
{
    return std::visit(overloaded{std::forward<Ts>(ts)...}, std::forward<Variant>(variant));
}

template<typename T>
decltype(auto) MUST_MOVE(T&& t)
{
    static_assert(std::is_move_constructible_v<std::remove_reference_t<T>>, "Type must be move constructible");
    static_assert(!std::is_const_v<std::remove_reference_t<T>>, "Cannot move a const object");
    return std::move(t);
}

namespace detail
{
template<class R, class T>
    requires std::integral<R> && std::floating_point<T>
constexpr R float_to_int_cast_core(T fx)
{
    assert(
      !std::isnan(fx) && static_cast<T>(std::numeric_limits<R>::lowest()) <= fx
      && fx <= static_cast<T>(std::numeric_limits<R>::max())
    );
    return static_cast<R>(fx);
}
} // namespace detail

template<class R, class T>
    requires std::integral<R> && std::floating_point<T>
constexpr R ifloor(T x)
{
    return detail::float_to_int_cast_core<R>(floor(x));
}

template<class R, class T>
    requires std::integral<R> && std::floating_point<T>
constexpr R iround(T x)
{
    return detail::float_to_int_cast_core<R>(round(x));
}

template<class R, class T>
    requires std::integral<R> && std::floating_point<T>
constexpr R iceil(T x)
{
    return detail::float_to_int_cast_core<R>(ceil(x));
}

template<class To, class From>
    requires std::integral<To> && std::integral<From>
constexpr To iicast(From f)
{
    assert(std::in_range<To>(f));
    return static_cast<To>(f);
}

template<class To, class From>
    requires std::floating_point<To> && std::floating_point<From>
constexpr To ffcast(From f)
{
    return static_cast<To>(f);
}

template<class To, class From>
    requires std::floating_point<To> && std::integral<From>
constexpr To ifcast(From f)
{
    return static_cast<To>(f);
}

template<class From>
    requires std::integral<From> && std::unsigned_integral<From>
constexpr auto uscast(From f)
{
    using To = typename std::make_signed<From>::type;
    assert(std::in_range<To>(f));
    return static_cast<To>(f);
}

template<class From>
    requires std::integral<From> && std::signed_integral<From>
constexpr auto sucast(From f)
{
    using To = typename std::make_unsigned<From>::type;
    assert(std::in_range<To>(f));
    return static_cast<To>(f);
}

template<class From>
    requires std::integral<From>
constexpr auto scast(From f)
{
    using To = typename std::make_signed<From>::type;
    assert(std::in_range<To>(f));
    return static_cast<To>(f);
}

template<class From>
    requires std::integral<From>
constexpr auto ucast(From f)
{
    using To = typename std::make_unsigned<From>::type;
    assert(std::in_range<To>(f));
    return static_cast<To>(f);
}

#if MEADOW_HAS_ABSL == 1
template<class R>
R TRY_OR_FAIL(std::expected<R, std::string> X)
{
    LOG_IF(FATAL, !X) << X.error();
    return MOVE(*X);
}

template<>
inline void TRY_OR_FAIL(std::expected<void, std::string> X)
{
    LOG_IF(FATAL, !X) << X.error();
}
#endif

// Hash

template<class T>
std::size_t hash_value(const T& v)
{
    return std::hash<std::decay_t<T>>{}(v);
}

template<class T>
void hash_combine(std::size_t& seed, const T& v)
{
    seed ^= hash_value(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<class It>
std::size_t hash_range(It first, It last)
{
    std::size_t seed = 0;
    for (; first != last; ++first) {
        hash_combine(seed, *first);
    }
    return seed;
}

template<class It>
void hash_range(std::size_t& seed, It first, It last)
{
    for (; first != last; ++first) {
        hash_combine(seed, *first);
    }
}

namespace std
{
template<class A, class B>
struct std::hash<std::pair<A, B>> {
    std::size_t operator()(std::pair<A, B> const& s) const noexcept
    {
        auto h = ::hash_value(s.first);
        ::hash_combine(h, s.second);
        return h;
    }
};

template<class T>
void remove_prefix(std::span<T>& s, size_t n)
{
    assert(n <= s.size());
    s = span(s.begin() + uscast(n), s.end());
}

template<class T>
void remove_postfix(std::span<T>& s, size_t n)
{
    assert(n <= s.size());
    s = span(s.begin(), s.end() - uscast(n));
}

} // namespace std
