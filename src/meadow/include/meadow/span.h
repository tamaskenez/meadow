#pragma once

#include "meadow/cppext.h"

template<class T>
auto span_from_data_size(T&& xs)
{
    return std::span(xs.data(), ucast(xs.size()));
}
