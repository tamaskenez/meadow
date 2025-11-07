#pragma once

#include <meadow/cppext.h>

expected<string, string> read_file_to_string(const fs::path& path);
[[nodiscard]] expected<void, string> write_string_to_file(string_view sv, const fs::path& path);
