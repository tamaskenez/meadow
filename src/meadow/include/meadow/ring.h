#pragma once

#include <utility>
#include <vector>

// Preallocated vector filling up and wrapping over, replacing old values in a circular manner when pushing items into
// it, keeping the last N items.
template<class T, size_t N>
class SaturatingRingVector
{
public:
    using vector_type = std::vector<T>;
    using reference = typename vector_type::reference;

    SaturatingRingVector()
    {
        vs.reserve(N);
    }
    void push_back(const T& x)
    {
        if (push_head > vs.size()) {
            vs.push_back(x);
        } else {
            vs[push_head] = x;
        }
        push_head = (push_head + 1) % N;
    }
    void push_back(T&& x)
    {
        if (push_head > vs.size()) {
            vs.push_back(std::move(x));
        } else {
            vs[push_head] = std::move(x);
        }
        push_head = (push_head + 1) % N;
    }
    template<class... Args>
    reference emplace_back(Args&&... args)
    {
        if (push_head > vs.size()) {
            vs.emplace_back(std::forward<Args...>(args...));
        } else {
            vs[push_head] = T(std::forward<Args...>(args...));
        }
        push_head = (push_head + 1) % N;
    }

private:
    vector_type vs;
    size_t push_head = 0;
};
