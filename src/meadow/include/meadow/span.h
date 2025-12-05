#pragma once

#include <array>
#include <mdspan>
#include <span>

template<class T>
auto span_from_data_size(T&& xs)
{
    return std::span(xs.data(), static_cast<size_t>(xs.size()));
}

template<class T>
auto mdspan_using_data_and_size(T&& xs)
{
    return std::mdspan(xs.data(), static_cast<size_t>(xs.size()));
}

template<class data_handle_type>
auto mdspan_from_data_size_stride(data_handle_type p, size_t size, size_t stride)
{
    using ExtentsType = std::dextents<size_t, 1>;
    return mdspan(p, std::layout_stride::mapping<ExtentsType>(ExtentsType(size), std::array<size_t, 1>({stride})));
}
