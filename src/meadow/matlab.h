#pragma once

#include "meadow/errno.h"

#include <cmath>
#include <concepts>

// Helper class, for example, to supply Eigen matrices for reading.
template<class T>
struct MatrixReader {
public:
    MatrixReader(size_t rowsArg, size_t colsArg, function<T(size_t r, size_t c)> atFnArg)
        : rows(rowsArg)
        , cols(colsArg)
        , atFn(MOVE(atFnArg))
    {
    }
    size_t rows, cols;
    function<T(size_t r, size_t c)> atFn;
};

namespace matlab
{
template<class T>
    requires std::floating_point<T>
T mag2db(T x)
{
    return static_cast<T>(20) * log10(x);
}

template<class T>
    requires std::floating_point<T>
T pow2db(T x)
{
    return static_cast<T>(10) * log10(x);
}

template<class T>
    requires std::floating_point<T>
T db2mag(T x)
{
    return pow(static_cast<T>(10), x / 20);
}

template<class T>
    requires std::integral<T> || std::floating_point<T>
[[nodiscard]] expected<void, string> saveAscii(const char* path, span<T> xs)
{
    FILE* f = fopen(path, "wt");
    if (!f) {
        return unexpected(format("{} ({})", strerror(errno), strerrno_or_int(errno)));
    }
    for (auto x : xs) {
        std::println(f, "{}", x);
    }
    CHECK(fclose(f) == 0);
    return {};
}

template<class T>
    requires std::integral<T> || std::floating_point<T>
[[nodiscard]] expected<void, string> saveAscii(const char* path, const MatrixReader<T>& mr)
{
    FILE* f = fopen(path, "wt");
    if (!f) {
        return unexpected(format("{} ({})", strerror(errno), strerrno_or_int(errno)));
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

} // namespace matlab
