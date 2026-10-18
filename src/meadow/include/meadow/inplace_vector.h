#pragma once

#include <version>

// Use the standard std::inplace_vector when available (C++26, e.g. GCC 16), otherwise a vendored implementation.
#ifdef __cpp_lib_inplace_vector
  #include <inplace_vector>
#else
  #include "detail/inplace_vector_impl.h"
#endif
