#include "meadow/string.h"

#include <algorithm>

std::string toupper(std::string_view s)
{
    std::string r(s);
    for (auto& c : r) {
        c = static_cast<char>(toupper(c));
    }
    return r;
}
