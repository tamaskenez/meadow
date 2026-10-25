#include "meadow/matlab.h"

#include "meadow/cppext.h"
#include "meadow/math.h"

#include <complex>
#include <system_error>

#if MEADOW_HAS_EIGEN == 1
  #include "meadow/eigen_dense.h"
#endif

namespace matlab
{
namespace detail
{
std::expected<void, std::string>
saveAscii_core(const std::filesystem::path& path, const std::function<void(FILE* f)>& fn)
{
    auto f_or = try_fopen(path.string().c_str(), "wt");
    if (!f_or) {
        return std::unexpected(
          format("{} ({})", std::system_category().message(f_or.error()), strerrno_or_int(f_or.error()))
        );
    }
    fn(*f_or);
    CHECK(fclose(*f_or) == 0);
    return {};
}
} // namespace detail

double sum(const vector<double>& xs)
{
    return std::accumulate(BEGIN_END(xs), 0.0);
}

void operator/=(vector<double>& xs, const double y)
{
    for (auto& x : xs) {
        x /= y;
    }
}

template<class T>
array<T, 2> polyfit1(span<const T> xs, span<const T> ys)
{
    // X = [1 xs[0]      Y = [ys[0]
    //      1 xs[1]           ys[1]
    //      ...    ];         ...  ];
    //
    // beta = (X'*X)^-1 * X'*Y
    //
    // X' * X = [  N     sum(xs)
    //           sum(xs) sum(xs^2)]
    //
    const auto N = xs.size();
    CHECK(ys.size() == N);
    assert(N >= 2);
    T sx = 0, sx2 = 0, sy = 0, sxy = 0;
    for (auto&& [x, y] : vi::zip(xs, ys)) {
        sx += x;
        sx2 += square(x);
        sy += y;
        sxy += x * y;
    }
    const auto N_T = ifcast<T>(N);
    auto det = N_T * sx2 - square(sx);
    // [sx2 -sx
    //  -sx  N] / det
    //
    // X'*Y = [sy
    //         sxy]
    //
    // beta = [sx2 * sy + sx * sxy
    //         sx * sy + N * sxy] / det
    return array<T, 2>{(N_T * sxy - sx * sy) / det, (sx2 * sy - sx * sxy) / det};
}

template array<float, 2> polyfit1(span<const float> xs, span<const float> ys);

template array<double, 2> polyfit1(span<const double> xs, span<const double> ys);

template<class C, class X>
decltype(std::declval<C>() * std::declval<X>()) polyval(std::span<const C> cs, X x)
{
    using R = decltype(std::declval<C>() * std::declval<X>());
    const auto N = cs.size();
    if (N == 0) {
        return R(0);
    }
    R s = cs.front();
    for (size_t i = 1; i < N; ++i) {
        s = (s * x) + cs[i];
    }
    return s;
}

template float polyval(std::span<const float> cs, float x);

template double polyval(std::span<const double> cs, double x);

template std::complex<double> polyval(std::span<const std::complex<double>> cs, std::complex<double> x);

template std::complex<double> polyval(std::span<const double> cs, std::complex<double> x);

template<class T>
std::vector<T> polyder(std::span<const T> cs)
{
    std::vector<T> r;
    if (cs.size() <= 1) {
        r.assign(1, 0.0);
    } else {
        r.resize(cs.size() - 1);
        detail::polyder_noalloc_core(cs, std::span<T>(r.begin(), r.end()));
    }
    return r;
}

template std::vector<float> polyder(std::span<const float> cs);

template std::vector<double> polyder(std::span<const double> cs);

namespace detail
{
template<class T>
void polyder_noalloc_core(std::span<const T> cs, std::span<T> r)
{
    const auto N = cs.size();
    CHECK(N >= 1 && r.size() == N - 1);
    if (N == 1) {
        r.front() = 0.0;
    } else {
        for (size_t i : vi_iota<size_t>(0, N - 1)) {
            r[i] = ifcast<T>(N - i - 1) * cs[i];
        }
    }
}

template void polyder_noalloc_core(std::span<const float> cs, std::span<float> r);

template void polyder_noalloc_core(std::span<const double> cs, std::span<double> r);
} // namespace detail

template<class T>
std::inplace_vector<T, 2> real_roots2(std::span<const T, 3> cs)
{
    const auto D = square(cs[1]) - 4 * cs[0] * cs[2];
    if (D < 0) {
        return {};
    }
    if (D > 0) {
        const auto sqrt_D = sqrt(D);
        auto r = std::inplace_vector<T, 2>({(-cs[1] - sqrt_D) / (2 * cs[0]), (-cs[1] + sqrt_D) / (2 * cs[0])});
        if (r[0] > r[1]) {
            std::swap(r[0], r[1]);
        }
        return r;
    }
    return std::inplace_vector<T, 2>({-cs[1] / (2 * cs[0])});
}

template std::inplace_vector<float, 2> real_roots2(std::span<const float, 3> cs);

template std::inplace_vector<double, 2> real_roots2(std::span<const double, 3> cs);

#if MEADOW_HAS_EIGEN == 1
std::vector<double> polyfit(
  std::mdspan<const double, std::dextents<size_t, 1>, std::layout_stride> xs,
  std::mdspan<const double, std::dextents<size_t, 1>, std::layout_stride> ys,
  int degree
)
{
    const auto n = iicast<int>(xs.extent(0));
    CHECK(cmp_equal(ys.extent(0), n));
    assert(n >= degree + 1);

    // Create Vandermonde matrix (n x (degree+1))
    Eigen::MatrixXd A(n, degree + 1);

    for (int i = 0; i < n; ++i) {
        const auto x = xs[i];
        double xp = 1.0;
        for (int p = 0; p <= degree; ++p) {
            A(i, p) = xp;
            xp *= x;
        }
    }

    // Create vector y.
    Eigen::VectorXd b(n);
    for (int i = 0; i < n; ++i) {
        b(i) = ys[i];
    }

    // Solve least squares: A * c = b
    // Using Householder QR (numerically stable)
    Eigen::VectorXd c = A.colPivHouseholderQr().solve(b);

    // Copy to std::vector
    std::vector<double> coeffs(sucast(c.size()));
    for (int i = 0; i < c.size(); ++i) {
        coeffs[sucast(c.size() - i - 1)] = c(i);
    }

    return coeffs;
}
#endif

std::vector<double> linspace(double x1, double x2, size_t n)
{
    std::vector<double> r;
    r.reserve(n);
    const double one_over_n_minus_1 = 1.0 / ifcast<double>(n - 1);
    for (size_t i = 0; i < n; ++i) {
        r.push_back(std::lerp(x1, x2, ifcast<double>(i) * one_over_n_minus_1));
    }
    return r;
}

std::vector<double> logspace(double e1, double e2, size_t n)
{
    std::vector<double> r;
    r.reserve(n);
    const double one_over_n_minus_1 = 1.0 / ifcast<double>(n - 1);
    for (size_t i = 0; i < n; ++i) {
        r.push_back(pow(10.0, std::lerp(e1, e2, ifcast<double>(i) * one_over_n_minus_1)));
    }
    return r;
}

namespace
{
// 1970-01-01, the day std::chrono counts from, as a MATLAB serial date number.
constexpr double k_reference_datenum_value = 730486.0;
constexpr auto k_reference_datenum_local_days =
  chr::local_days(chr::year_month_day{chr::year(2000), chr::month{1}, chr::day{1}});
} // namespace

double datenum(chr::local_days d)
{
    return ifcast<double>((d - k_reference_datenum_local_days).count()) + k_reference_datenum_value;
}

double datenum(chr::year_month_day ymd)
{
    assert(ymd.ok());
    return datenum(chr::local_days(ymd));
}

double datenum(int y, int m, int d)
{
    return datenum(chr::year_month_day{chr::year(y), chr::month(iicast<unsigned>(m)), chr::day(iicast<unsigned>(d))});
}

namespace
{
struct DeviationSums {
    double sxx, sxy, syy;
};

// Divisor of the deviation sums for the normalization argument w.
double normalizationDivisor(size_t n, int w)
{
    CHECK(w == 0 || w == 1);
    return ifcast<double>(n) - (w == 0 ? 1.0 : 0.0);
}

// Sums of the squared and cross deviations from the means of xs and ys.
DeviationSums deviationSums(span<const double> xs, span<const double> ys)
{
    CHECK(xs.size() == ys.size());
    const auto mean_x = mean(xs);
    const auto mean_y = mean(ys);
    DeviationSums r{0, 0, 0};
    for (auto&& [x, y] : vi::zip(xs, ys)) {
        const auto dx = x - mean_x;
        const auto dy = y - mean_y;
        r.sxx += square(dx);
        r.sxy += dx * dy;
        r.syy += square(dy);
    }
    return r;
}
} // namespace

array<array<double, 2>, 2> cov(span<const double> xs, span<const double> ys, int w)
{
    const auto s = deviationSums(xs, ys);
    const auto norm = normalizationDivisor(xs.size(), w);
    return {
      array<double, 2>{s.sxx / norm, s.sxy / norm},
       array<double, 2>{s.sxy / norm, s.syy / norm}
    };
}

double var(span<const double> xs, int w)
{
    const auto norm = normalizationDivisor(xs.size(), w);
    const auto mean_x = mean(xs);
    double sxx = 0;
    for (auto x : xs) {
        sxx += square(x - mean_x);
    }
    return sxx / norm;
}

double std(span<const double> xs, int w)
{
    return sqrt(var(xs, w));
}
} // namespace matlab
