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
    const double N = L + 1;
    return 0.5 * (1 - cos(2 * num::pi * n / N)) * exp(-alpha * abs(N - 2 * n) / N);
}

} // namespace matlab
