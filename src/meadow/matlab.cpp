#include "meadow/matlab.h"
#include "meadow/math.h"

#if MEADOW_HAS_CYL_BESSEL_I == 0 && MEADOW_HAS_BOOST == 1
  #include <boost/math/special_functions/bessel.hpp>
#endif

namespace matlab
{
double rectwin_fn(int n, int L)
{
    return 0 <= n && n < L ? 1.0 : 0.0;
}
double blackman_fn(int n, int L)
{
    if (n < 0 || L <= n) {
        return 0.0;
    }
    return 0.42 - 0.5 * cos(2 * num::pi * n / (L - 1)) + 0.08 * cos(4 * num::pi * n / (L - 1));
}

double gausswin_fn(int n, int L, double alpha)
{
    if (n < 0 || L <= n) {
        return 0.0;
    }
    const double center = (double(L) - 1) / 2;
    const double dn = n - center;
    return exp(-0.5 * square(alpha * dn / center));
}

#if MEADOW_HAS_CYL_BESSEL_I == 1 || MEADOW_HAS_BOOST == 1
double kaiser_fn(int n, int L, double beta)
{
    if (n < 0 || L <= n) {
        return 0.0;
    }
    const double N_over_2 = (double(L) - 1) / 2;

  #ifdef HAVE_CYL_BESSEL_I
    using std::cyl_bessel_i;
  #else
    using boost::math::cyl_bessel_i;
  #endif

    return cyl_bessel_i(0.0, beta * sqrt(1 - square((n - N_over_2) / N_over_2))) / cyl_bessel_i(0.0, beta);
}
#endif

double hann_poisson_fn(int n, int L, double alpha)
{
    if (n < 0 || L <= n) {
        return 0.0;
    }
    const double N = L - 1;
    return 0.5 * (1 - cos(2 * num::pi * n / N)) * exp(-alpha * abs(N - 2 * n) / N);
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
    auto det = N * sx2 - square(sx);
    // [sx2 -sx
    //  -sx  N] / det
    //
    // X'*Y = [sy
    //         sxy]
    //
    // beta = [sx2 * sy + sx * sxy
    //         sx * sy + N * sxy] / det
    return array<T, 2>{(N * sxy - sx * sy) / det, (sx2 * sy - sx * sxy) / det};
}

template array<float, 2> polyfit1(span<const float> xs, span<const float> ys);
template array<double, 2> polyfit1(span<const double> xs, span<const double> ys);

} // namespace matlab
