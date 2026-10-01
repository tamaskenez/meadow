#pragma once

#include <meadow/cppext.h>

enum class IfMissing {
    error,
    return_empty
};

// Returns the contents of the file. A missing file is an error or an empty string, depending on `if_missing`.
NODIS expected<string, string> read_file_to_string(const fs::path& path, IfMissing if_missing = IfMissing::error);
NODIS expected<void, string> write_string_to_file(string_view sv, const fs::path& path);
