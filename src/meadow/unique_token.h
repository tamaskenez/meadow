#pragma once

// Simple, non-copyable, movable class which is either in default state (valid) or moved out (valid() == false).
// Can be used as an internal member, for example in a class representing a resource where it saves writing the
// copy/move/ctor/assignment functions.
//
// Example:
//
//     class SomeResource {
//         int resource_id;
//         unique_token token;
//         explicit SomeResource(int resource_id_arg): resource_id(resource_id_arg) {}
//     public:
//         // In move-ctor and move-assignments, resource_id will be copied but token will be moved,
//         // default implementation is fine.
//         SomeResource(MixIsInitialized&&) = default;
//         SomeResource& operator=(MixIsInitialized&&) = default;
//
//         ~SomeResource() {
//             if (token) {
//                 ... release the resource ...
//             }
//         }
//     };
class unique_token
{
public:
    unique_token() = default;
    unique_token(const unique_token&) = delete;
    unique_token(unique_token&& y)
        : valid_(y.valid_)
    {
        y.valid_ = false;
    }
    unique_token& operator=(const unique_token&) = delete;
    unique_token& operator=(unique_token&& y)
    {
        valid_ = y.valid_;
        y.valid_ = false;
        return *this;
    }
    operator bool() const
    {
        return valid_;
    }
    bool valid() const
    {
        return valid_;
    }

private:
    bool valid_ = true;
};
