#pragma once

#include "detail/float_to_int_cast_core.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <concepts>
#include <span>
#include <utility>
#include <vector>

template<class T>
constexpr T square(T x)
{
    return x * x;
}

// Fit a parabola through (-1, ym1), (0, y0), (1, yp1) and return the point where the derivative is zero.
// Precond: (ym1 + yp1) / 2 - y0 != 0
std::pair<double, double> extremumOfParabola(double ym1, double y0, double yp1);

#if MEADOW_HAS_EIGEN == 1
// Fit a parabola through xs, ys and return the point where the derivative is zero.
template<class R, class X, class Y>
std::pair<R, R> extremumOfParabola(std::span<const X> xs, std::span<const Y> ys);
#endif

template<class T>
    requires std::integral<T>
constexpr bool is_even(T x)
{
    return (x & 1) == 0;
}

template<class T>
    requires std::integral<T>
constexpr bool is_odd(T x)
{
    return (x & 1) == 1;
}

template<class T>
    requires std::floating_point<T>
T sgn(T x)
{
    switch (std::fpclassify(x)) {
    case FP_NAN:
    case FP_ZERO:
        return x;
    default:
        return std::signbit(x) ? T(-1) : T(1);
    }
}

template<class T>
    requires std::signed_integral<T>
T sgn(T x)
{
    return x > T(0) ? T(1) : x >> (sizeof(T) * 8 - 1);
}

template<class T>
    requires std::unsigned_integral<T>
T sgn(T x)
{
    return x > T(0) ? T(1) : T(0);
}

// Negative x will be correctly handled, resulting in "modulo" not remainder. Undefined behavior if y <= 0.
template<class T>
    requires std::floating_point<T>
T modulo(T x, T y)
{
    assert(y > 0);
    return std::fmod(std::fmod(x, y) + y, y);
}

// Negative x will be correctly handled, resulting in "modulo" not remainder. Undefined behavior if y <= 0.
template<class X, class Y>
    requires std::integral<X> && std::integral<Y>
auto modulo(X x, Y y)
{
    assert(y > 0);
    return (x % y + y) % y;
}

template<class X, class L, class H>
constexpr bool in_cc_range(const X& x, const L& lo, const H& hi)
{
    if consteval {
        if (!(lo <= hi)) {
            throw std::invalid_argument("Invalid range: lo must be less than or equal to hi");
        }
    } else {
        assert(lo <= hi);
    }
    return lo <= x && x <= hi;
}

template<class X, class L, class H>
bool in_co_range(const X& x, const L& lo, const H& hi)
{
    if constexpr (std::is_integral_v<L> && std::is_integral_v<H>) {
        assert(cmp_less_equal(lo, hi));
    } else {
        assert(lo <= hi);
    }
    return lo <= x && x < hi;
}

template<class X, class L, class H>
bool in_oo_range(const X& x, const L& lo, const H& hi)
{
    assert(lo <= hi); // Note: lo == hi is allowed but result is always false.
    return lo < x && x < hi;
}

template<class T>
    requires std::floating_point<T>
T minus_round(T x)
{
    return x - round(x);
}

template<class X>
    requires std::floating_point<X>
constexpr bool equal_epsilon(X x, X y, X eps)
{
    return in_cc_range(x - y, -eps, eps);
}

template<class X>
    requires std::floating_point<X>
constexpr bool equal_epsilon(span<const X> x, span<const X> y, X eps)
{
    if (x.size() != y.size()) {
        return false;
    }
    for (size_t i = 0; i < x.size(); ++i) {
        if (!equal_epsilon(x[i], y[i], eps)) {
            return false;
        }
    }
    return true;
}

// Round to specified decimal places after the decimal point (num_digits >= 0).
// If num_digits < 0, rounds to multiplies of pow(10, -num_digits).
template<class F>
    requires std::floating_point<F>
F round_to_digits(F x, int num_digits)
{
    const auto unit = pow(10.0, num_digits);
    return ffcast<F>(round(x * unit) / unit);
}

template<class T>
    requires std::floating_point<T>
std::vector<T> regspace(T begin, T step, T end)
{
    std::vector<T> result;
    if (step < T(0)) {
        if (begin < end) {
            assert(false);
        } else {
            auto n = ifloor<size_t>((end - begin) / step) + 1;
            result.reserve(n);
            for (size_t i = 0; i < n; ++i) {
                result.push_back(std::max(end, begin + ifcast<T>(i) * step));
            }
        }
    } else if (T(0) < step) {
        if (begin > end) {
            assert(false);
        } else {
            auto n = ifloor<size_t>((end - begin) / step) + 1;
            result.reserve(n);
            for (size_t i = 0; i < n; ++i) {
                result.push_back(std::min(end, begin + ifcast<T>(i) * step));
            }
        }
    } else {
        assert(false);
    }
    return result;
}

template<class T>
    requires std::floating_point<T>
pair<T, T> floor_frac(T x)
{
    const auto integral = floor(x);
    return pair(integral, x - integral);
}

template<class I, class T>
    requires std::floating_point<T>
pair<I, T> ifloor_frac(T x)
{
    const auto integral = floor(x);
    return pair(detail::float_to_int_cast_core<I>(integral), x - integral);
}

enum class VarianceNorm {
    sample,    // Normalize by count() - 1
    population // Normalize by count()
};

// Statistics of a continuously sampled process, updated incrementally, without storing the samples.
class RunningStat
{
public:
    // Adds a sample. Samples must be finite.
    void operator()(double sample);
    void reset();

    NODIS size_t count() const;

    // Precond: count() > 0. Return NaN if the precondition is not met.
    NODIS double mean() const;
    NODIS double min() const;
    NODIS double max() const;
    NODIS double range() const;
    // Precond: count() > 1 with VarianceNorm::sample, count() > 0 with VarianceNorm::population. Return NaN if the
    // precondition is not met.
    NODIS double var(VarianceNorm norm = VarianceNorm::sample) const;
    NODIS double stddev(VarianceNorm norm = VarianceNorm::sample) const;

private:
    size_t num_samples = 0;
    double running_mean = 0;
    double sum_sq_dev = 0; // Sum of the squared deviations from the running mean.
    double min_sample = INFINITY;
    double max_sample = -INFINITY;
};
