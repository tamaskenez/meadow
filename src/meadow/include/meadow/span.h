#pragma once

#include <mdspan>
#include <span>

template<class T>
auto span_from_data_size(T&& xs)
{
    return std::span(xs.data(), static_cast<size_t>(xs.size()));
}

template<class T>
auto mdspan_from_data_size(T&& xs)
{
    return std::mdspan(xs.data(), static_cast<size_t>(xs.size()));
}
