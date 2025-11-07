#include "meadow/file.h"
#include "meadow/errno.h"

expected<string, string> read_file_to_string(const fs::path& path)
{
    std::error_code ec;
    auto size = file_size(path, ec);
    if (ec) {
        return unexpected(ec.message());
    }
    string result(size, 0);
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) {
        if (errno) {
            return unexpected(strerrno_or_int(errno));
        }
        return unexpected("fopen failed");
    }
    auto bytes_read = fread(result.data(), 1, size, f);
    fclose(f);
    if (bytes_read != size) {
        return unexpected(format("read {} bytes instead of {}", bytes_read, size));
    }
    return result;
}

expected<void, string> write_string_to_file(string_view sv, const fs::path& path)
{
    FILE* f = fopen(path.c_str(), "wb");
    if (!f) {
        if (errno) {
            return unexpected(strerrno_or_int(errno));
        }
        return unexpected("fopen failed");
    }
    auto bytes_written = fwrite(sv.data(), 1, sv.size(), f);
    fclose(f);
    if (bytes_written != sv.size()) {
        return unexpected(format("wrote {} bytes instead of {}", bytes_written, sv.size()));
    }
    return {};
}
