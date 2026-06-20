#include "meadow/filesystem.h"

expected<fs::path, std::error_code> try_temp_directory_path()
{
    std::error_code ec;
    auto r = fs::temp_directory_path(ec);
    if (ec) {
        return unexpected(ec);
    }
    return r;
}
