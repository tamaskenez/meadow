#pragma once

#include "meadow/cppext.h"
#include "meadow/errno.h"

#include <bit>
#include <meadow/inplace_vector.h>

// Helper class, for example, to supply Eigen matrices for reading.
template<class T>
struct MatrixReader {
public:
    MatrixReader(size_t rowsArg, size_t colsArg, std::function<T(size_t r, size_t c)> atFnArg)
        : rows(rowsArg)
        , cols(colsArg)
        , atFn(MOVE(atFnArg))
    {
    }
    size_t rows, cols;
    std::function<T(size_t r, size_t c)> atFn;
};

namespace matlab
{
template<class T>
    requires std::floating_point<T>
T mag2db(T x)
{
    assert(x >= 0);
    return x == 0 ? static_cast<T>(-INFINITY) : static_cast<T>(20) * log10(x);
}

template<class T>
    requires std::floating_point<T>
T pow2db(T x)
{
    assert(x >= 0);
    return x == 0 ? static_cast<T>(-INFINITY) : static_cast<T>(10) * log10(x);
}

template<class T>
    requires std::floating_point<T>
T db2mag(T x)
{
    return pow(static_cast<T>(10), x / 20);
}

template<class T>
    requires std::floating_point<T>
T db2pow(T x)
{
    return pow(static_cast<T>(10), x / 10);
}

template<class T>
    requires std::floating_point<T> || std::integral<T>
constexpr auto rad2deg(T x)
{
    using R = decltype(std::sin(x));
    return x * static_cast<R>(180.0 / num::pi);
}

template<class T>
    requires std::floating_point<T> || std::integral<T>
constexpr auto deg2rad(T x)
{
    using R = decltype(std::sin(x));
    return x * static_cast<R>(num::pi / 180.0);
}

template<class T>
    requires std::integral<T> || std::floating_point<T>
[[nodiscard]] std::expected<void, std::string> saveAscii(const std::filesystem::path& path, std::span<T> xs)
{
    FILE* f = fopen(path.c_str(), "wt");
    if (!f) {
        return std::unexpected(format("{} ({})", strerror(errno), strerrno_or_int(errno)));
    }
    for (auto x : xs) {
        std::println(f, "{}", x);
    }
    CHECK(fclose(f) == 0);
    return {};
}

template<class T>
    requires std::integral<T> || std::floating_point<T>
[[nodiscard]] std::expected<void, std::string> saveAscii(const char* path, const MatrixReader<T>& mr)
{
    FILE* f = fopen(path, "wt");
    if (!f) {
        return std::unexpected(format("{} ({})", strerror(errno), strerrno_or_int(errno)));
    }
    for (size_t r = 0; r < mr.rows; ++r) {
        for (size_t c = 0; c < mr.cols; ++c) {
            std::print(f, "{} ", mr.atFn(r, c));
        }
        std::println(f);
    }
    CHECK(fclose(f) == 0);
    return {};
}

template<class R, class X>
R mean(X&& xs)
{
    CHECK(!xs.empty());
    return ra::fold_left(
             std::begin(xs) + 1,
             std::end(xs),
             static_cast<R>(*std::begin(xs)),
             [](R a, auto b) -> R {
                 return a + static_cast<R>(b);
             }
           )
         / std::size(xs);
}

template<class X>
    requires std::floating_point<std::decay_t<decltype(*std::begin(std::declval<X>()))>>
auto mean(X&& xs)
{
    return mean<std::decay_t<decltype(*std::begin(xs))>>(xs);
}

template<class X, class V, class XQ>
    requires std::floating_point<XQ>
auto interp1(X&& x, V&& v, XQ xq) -> std::decay_t<decltype(*std::begin(v))>
{
    if (std::size(x) < 1) {
        assert(false);
        return NAN;
    }
    assert(cmp_equal(std::size(x), std::size(v)));
    auto it = ra::lower_bound(std::begin(x), std::end(x), xq);
    if (it == std::begin(x)) {
        return *std::begin(v);
    }
    if (it == std::end(x)) {
        return *std::prev(std::end(v));
    }
    const auto j = it - std::begin(x);
    assert(j >= 1);
    const auto i = j - 1;

    const auto xi = *(std::begin(x) + i);
    const auto xj = *(std::begin(x) + j);
    return std::lerp(*(std::begin(v) + i), *(std::begin(v) + j), (xq - xi) / (xj - xi));
}

double rectwin_fn(int n, int L);
std::vector<double> blackman(int L);
double blackman_fn(int n, int L);
double gausswin_fn(int n, int L, double alpha);

#if MEADOW_HAS_CYL_BESSEL_I == 1 || MEADOW_HAS_BOOST == 1
double kaiser_fn(int n, int L, double beta);
#endif

double hann_poisson_fn(int n, int L, double alpha);

template<class T>
T nextpow2(T x)
{
    if constexpr (std::is_signed_v<T>) {
        if (x == std::numeric_limits<T>::min()) {
            return static_cast<T>(sizeof(T) * 8 - 1);
        }
        if (x < 0) {
            x = -x;
        }
        const auto ux = static_cast<std::make_unsigned_t<T>>(x);
        return static_cast<T>(nextpow2(ux));
    } else {
        constexpr T k_highest_power_of_2 = T(1) << (sizeof(T) * 8 - 1);
        if (x & k_highest_power_of_2) {
            return x == k_highest_power_of_2 ? sizeof(T) * 8 - 1 : sizeof(T) * 8;
        }
        return static_cast<T>(
          static_cast<T>(sizeof(T) * 8) - static_cast<T>(std::countl_zero(std::bit_ceil(x))) - T(1)
        );
    }
}

double sum(const vector<double>& xs);
void operator/=(vector<double>& xs, const double y);

template<class T>
std::array<T, 2> polyfit1(std::span<const T> xs, std::span<const T> ys);

template<class T>
T polyval(std::span<const T> cs, T x);

template<class T>
std::vector<T> polyder(std::span<const T> cs);

template<class T>
std::inplace_vector<T, 2> real_roots2(std::span<const T, 3> cs);
} // namespace matlab
