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
    auto f_or = try_fopen(path.string().c_str(), "rb");
    if (!f_or) {
        return unexpected(strerrno_or_int(f_or.error()));
    }
    auto bytes_read = fread(result.data(), 1, size, *f_or);
    fclose(*f_or);
    if (bytes_read != size) {
        return unexpected(format("read {} bytes instead of {}", bytes_read, size));
    }
    return result;
}

expected<void, string> write_string_to_file(string_view sv, const fs::path& path)
{
    auto f_or = try_fopen(path.string().c_str(), "wb");
    if (!f_or) {
        return unexpected(strerrno_or_int(f_or.error()));
    }
    auto bytes_written = fwrite(sv.data(), 1, sv.size(), *f_or);
    fclose(*f_or);
    if (bytes_written != sv.size()) {
        return unexpected(format("wrote {} bytes instead of {}", bytes_written, sv.size()));
    }
    return {};
}
