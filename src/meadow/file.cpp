#include "meadow/file.h"

#include <fstream>

expected<string, string> read_file_to_string(const fs::path& path, IfMissing if_missing)
{
    std::error_code ec;
    const auto size = fs::file_size(path, ec);
    if (ec) {
        if (ec == std::errc::no_such_file_or_directory && if_missing == IfMissing::return_empty) {
            return string();
        }
        return unexpected(format("Can't access {}: {}", path.string(), ec.message()));
    }
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        return unexpected(format("Can't open {}", path.string()));
    }
    string result(size, '\0');
    f.read(result.data(), iicast<std::streamsize>(size));
    if (!f) {
        return unexpected(format("Can't read {}: read {} bytes instead of {}", path.string(), f.gcount(), size));
    }
    return result;
}

expected<void, string> write_string_to_file(string_view sv, const fs::path& path)
{
    std::ofstream f(path, std::ios::binary | std::ios::trunc);
    if (!f) {
        return unexpected(format("Can't open {} for writing", path.string()));
    }
    f.write(sv.data(), iicast<std::streamsize>(sv.size()));
    f.close();
    if (!f) {
        return unexpected(format("Can't write {}", path.string()));
    }
    return {};
}
