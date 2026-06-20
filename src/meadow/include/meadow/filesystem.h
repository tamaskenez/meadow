#pragma once

#include "meadow/cppext.h"

NODIS expected<fs::path, std::error_code> try_temp_directory_path();
