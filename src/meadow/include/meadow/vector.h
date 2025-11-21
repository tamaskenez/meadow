#pragma once

#include <algorithm>
#include <cassert>
#include <vector>

template<class T>
void erase_with_back(std::vector<T>& vs, typename std::vector<T>::iterator pos)
{
    assert(!vs.empty());
    auto it_back = std::prev(vs.end());
    if (pos != it_back) {
        std::iter_swap(pos, it_back);
    }
    vs.erase(it_back);
}
