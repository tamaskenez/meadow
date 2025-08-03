#include "meadow/math.h"

#include "meadow/matlab.h"

#include <Eigen/Dense>

std::pair<double, double> extremumOfParabola(double ym1, double y0, double yp1)
{
    const double a = (ym1 + yp1) / 2 - y0;
    const double b = (yp1 - ym1) / 2;
    const double x = -b / (2 * a);
    return std::pair(x, (a * x + b) * x + y0);
}

template<class R, class X, class Y>
std::pair<R, R> extremumOfParabola(span<const X> xs, span<const Y> ys)
{
    assert(xs.size() == ys.size());
    assert(xs.size() >= 3);
    if (xs.size() == 3) {
        const double x0 = double(xs[0]) - double(xs[1]);
        const double x2 = double(xs[2]) - double(xs[1]);
        const double y0 = double(ys[0]) - double(ys[1]);
        const double y2 = double(ys[2]) - double(ys[1]);

        const double D = square(x0) * x2 - x0 * square(x2);

        const double a = (x2 * y0 - x0 * y2) / D;
        const double b = (-square(x2) * y0 + square(x0) * y2) / D;

        const double x = -b / (2 * a);
        const double y = (a * x + b) * x;
        return std::pair(ffcast<R>(x + xs[1]), ffcast<R>(y + ys[1]));
    } else {
        Eigen::MatrixXd A(xs.size(), 3);
        Eigen::VectorXd B(xs.size());
        const double meanxs = matlab::mean<double>(xs);
        const double meanys = matlab::mean<double>(ys);
        for (size_t i = 0; i < xs.size(); ++i) {
            const double xi = xs[i] - meanxs;
            A(uscast(i), 0) = square(xi);
            A(uscast(i), 1) = xi;
            A(uscast(i), 2) = 1;
            B(uscast(i)) = ys[i] - meanys;
        }
        Eigen::Vector3d coeffs = A.colPivHouseholderQr().solve(B);
        const double x = -coeffs(1) / (2 * coeffs(0));
        const double y = (coeffs(0) * x + coeffs(1)) * x + coeffs(2);
        return std::pair(ffcast<R>(x + meanxs), ffcast<R>(y + meanys));
    }
}

template std::pair<float, float> extremumOfParabola(span<const int> xs, span<const float> ys);
template std::pair<float, float> extremumOfParabola(span<const float> xs, span<const float> ys);
template std::pair<double, double> extremumOfParabola(span<const double> xs, span<const double> ys);
