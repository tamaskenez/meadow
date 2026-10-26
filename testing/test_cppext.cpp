#include "meadow/cppext.h"

#include <gtest/gtest.h>

TEST(cppext, ifloor)
{
    static_assert(std::is_same_v<decltype(ifloor<uint16_t>(0.0)), uint16_t>);
    EXPECT_EQ(ifloor<int>(-1.1), -2);
    EXPECT_EQ(ifloor<int>(-1.0), -1);
    EXPECT_EQ(ifloor<int>(-0.9), -1);
    EXPECT_EQ(ifloor<int>(-0.5), -1);
    EXPECT_EQ(ifloor<int>(-0.1), -1);
    EXPECT_EQ(ifloor<int>(0.0), 0);
    EXPECT_EQ(ifloor<int>(0.1), 0);
    EXPECT_EQ(ifloor<int>(0.5), 0);
    EXPECT_EQ(ifloor<int>(0.9), 0);
    EXPECT_EQ(ifloor<int>(1.0), 1);
    EXPECT_EQ(ifloor<int>(1.1), 1);
}

TEST(cppext, iround)
{
    static_assert(std::is_same_v<decltype(iround<uint16_t>(0.0)), uint16_t>);
    EXPECT_EQ(iround<int>(-1.1), -1);
    EXPECT_EQ(iround<int>(-1.0), -1);
    EXPECT_EQ(iround<int>(-0.9), -1);
    EXPECT_EQ(iround<int>(-0.5), -1);
    EXPECT_EQ(iround<int>(-0.1), 0);
    EXPECT_EQ(iround<int>(0.0), 0);
    EXPECT_EQ(iround<int>(0.1), 0);
    EXPECT_EQ(iround<int>(0.5), 1);
    EXPECT_EQ(iround<int>(0.9), 1);
    EXPECT_EQ(iround<int>(1.0), 1);
    EXPECT_EQ(iround<int>(1.1), 1);
}

TEST(cppext, iceil)
{
    static_assert(std::is_same_v<decltype(iceil<uint16_t>(0.0)), uint16_t>);
    EXPECT_EQ(iceil<int>(-1.1), -1);
    EXPECT_EQ(iceil<int>(-1.0), -1);
    EXPECT_EQ(iceil<int>(-0.9), 0);
    EXPECT_EQ(iceil<int>(-0.5), 0);
    EXPECT_EQ(iceil<int>(-0.1), 0);
    EXPECT_EQ(iceil<int>(0.0), 0);
    EXPECT_EQ(iceil<int>(0.1), 1);
    EXPECT_EQ(iceil<int>(0.5), 1);
    EXPECT_EQ(iceil<int>(0.9), 1);
    EXPECT_EQ(iceil<int>(1.0), 1);
    EXPECT_EQ(iceil<int>(1.1), 2);
}

TEST(cppext, iicast)
{
    static_assert(std::is_same_v<decltype(iicast<uint16_t>(int8_t(0))), uint16_t>);
    EXPECT_EQ(iicast<uint8_t>(255), uint8_t(255));
}

namespace
{
void expect_same_span(span<int> a, span<int> b)
{
    EXPECT_EQ(a.size(), b.size());
    if (!a.empty()) {
        EXPECT_EQ(a.data(), b.data());
    }
}
} // namespace

TEST(cppext, remove_prefix)
{
    vector<int> xs = {12, 34, 54};
    const auto s0 = span(BEGIN_END(xs));
    for (size_t i = 0; i <= 3; ++i) {
        auto s1 = s0;
        remove_prefix(s1, i);
        expect_same_span(s0.subspan(i, 3 - i), s1);
    }
}

TEST(cppext, remove_postfix)
{
    vector<int> xs = {12, 34, 54};
    const auto s0 = span(BEGIN_END(xs));
    for (size_t i = 0; i <= 3; ++i) {
        auto s1 = s0;
        remove_postfix(s1, i);
        expect_same_span(s0.subspan(0, 3 - i), s1);
    }
}

template<class X, class Y>
void test_signed_subtract_2()
{
    const auto x_min = std::numeric_limits<X>::min();
    const auto x_max = std::numeric_limits<X>::max();
    const auto y_min = std::numeric_limits<Y>::min();
    const auto y_max = std::numeric_limits<Y>::max();
    ASSERT_EQ(signed_subtract(x_min, y_max), iicast<int64_t>(x_min) - iicast<int64_t>(y_max));
    ASSERT_EQ(signed_subtract(x_max, y_min), iicast<int64_t>(x_max) - iicast<int64_t>(y_min));
}

template<class X>
void test_signed_subtract_1()
{
    test_signed_subtract_2<X, uint8_t>();
    test_signed_subtract_2<X, uint16_t>();
    test_signed_subtract_2<X, uint32_t>();
    test_signed_subtract_2<X, int8_t>();
    test_signed_subtract_2<X, int16_t>();
    test_signed_subtract_2<X, int32_t>();
}

TEST(cppext, signed_subtract)
{
    test_signed_subtract_1<uint8_t>();
    test_signed_subtract_1<uint16_t>();
    test_signed_subtract_1<uint32_t>();
    test_signed_subtract_1<int8_t>();
    test_signed_subtract_1<int16_t>();
    test_signed_subtract_1<int32_t>();

    ASSERT_EQ(signed_subtract(uint64_t(3), uint64_t(2)), 1);
    ASSERT_EQ(signed_subtract(uint64_t(2), uint64_t(3)), -1);
}

TEST(cppext, PP_CAT)
{
    // Basic token concatenation
    constexpr int foobar = 42;
    static_assert(MEADOW_PP_CAT(foo, bar) == 42);

    // Numeric tokens
    static_assert(MEADOW_PP_CAT(1, 2) == 12);

    // Macro arguments are expanded before concatenation
#define PP_CAT_TEST_A foo
#define PP_CAT_TEST_B bar
    static_assert(MEADOW_PP_CAT(PP_CAT_TEST_A, PP_CAT_TEST_B) == 42);
#undef PP_CAT_TEST_A
#undef PP_CAT_TEST_B
}

TEST(cppext, PP_STRINGIZE)
{
    // Basic stringization
    static_assert(std::string_view(MEADOW_PP_STRINGIZE(hello)) == "hello");

    // Macro arguments are expanded before stringization
#define PP_STRINGIZE_TEST_A hello
    static_assert(std::string_view(MEADOW_PP_STRINGIZE(PP_STRINGIZE_TEST_A)) == "hello");
#undef PP_STRINGIZE_TEST_A
}

TEST(cppext, indices_of)
{
    const vector<int> xs = {10, 20, 30};
    static_assert(std::is_same_v<ra::range_value_t<decltype(indices_of(xs))>, size_t>);
    vector<size_t> ks;
    for (auto k : indices_of(xs)) {
        ks.push_back(k);
    }
    EXPECT_EQ(ks, (vector<size_t>{0, 1, 2}));
    EXPECT_EQ(ra::size(indices_of(xs)), xs.size());

    // Empty range.
    EXPECT_TRUE(indices_of(vector<int>{}).empty());

    // Index type follows the range's size type.
    const auto iv = vi::iota(5, 8);
    using iv_size_t = decltype(ra::size(iv));
    static_assert(std::is_same_v<ra::range_value_t<decltype(indices_of(iv))>, iv_size_t>);
    EXPECT_TRUE(ra::equal(indices_of(iv), vector<iv_size_t>{0, 1, 2}));

    // Other sized ranges.
    constexpr std::array<int, 4> arr{};
    EXPECT_EQ(ra::size(indices_of(arr)), 4u);
    int c_arr[2] = {};
    EXPECT_EQ(ra::size(indices_of(c_arr)), 2u);
    EXPECT_EQ(ra::size(indices_of(std::span(xs).subspan(1))), 2u);
}

TEST(cppext, enumerate)
{
    vector<string> xs = {"a", "b", "c"};
    vector<pair<size_t, string>> result;
    for (auto [i, x] : vi::enumerate(xs)) {
        result.emplace_back(iicast<size_t>(i), x);
    }
    EXPECT_EQ(
      result,
      (vector<pair<size_t, string>>{
        {0, "a"},
        {1, "b"},
        {2, "c"}
    })
    );

    // Elements are references into the underlying range.
    for (auto [i, x] : vi::enumerate(xs)) {
        x += std::to_string(i);
    }
    EXPECT_EQ(xs, (vector<string>{"a0", "b1", "c2"}));

    // Const range.
    const vector<int> cxs = {5, 6};
    for (auto [i, x] : vi::enumerate(cxs)) {
        static_assert(std::is_same_v<decltype(x), const int&>);
        EXPECT_EQ(x, cxs[iicast<size_t>(i)]);
    }

    // Empty range.
    EXPECT_TRUE(vi::enumerate(vector<int>{}).empty());

    // Rvalue range is owned by the view.
    auto owning = vi::enumerate(vector<int>{7, 8, 9});
    int sum = 0;
    for (auto [i, x] : owning) {
        sum += iicast<int>(i) * x;
    }
    EXPECT_EQ(sum, 0 * 7 + 1 * 8 + 2 * 9);

    // Composes with other views.
    auto evens = vi::enumerate(vector<int>{3, 4, 5, 6}) | vi::filter([](const auto& p) {
                     return std::get<0>(p) % 2 == 0;
                 })
               | vi::transform([](const auto& p) {
                     return std::get<1>(p);
                 });
    EXPECT_TRUE(ra::equal(evens, vector<int>{3, 5}));
}

TEST(cppext, enumerate_index_type)
{
    // Index is the range's difference type, as in std::views::enumerate.
    vector<int> xs = {1, 2};
    using E = decltype(vi::enumerate(xs));
    static_assert(std::is_same_v<std::tuple_element_t<0, ra::range_value_t<E>>, ptrdiff_t>);
    static_assert(std::is_same_v<std::tuple_element_t<0, ra::range_reference_t<E>>, ptrdiff_t>);

    using E8 = decltype(vi::enumerate(vi::iota(int8_t{0}, int8_t{3})));
    static_assert(std::is_same_v<
                  std::tuple_element_t<0, ra::range_value_t<E8>>,
                  ra::range_difference_t<decltype(vi::iota(int8_t{0}, int8_t{3}))>>);
}

TEST(cppext, enumerate_sized_common)
{
    vector<int> xs = {7, 8, 9};
    auto e = vi::enumerate(xs);
    static_assert(ra::sized_range<decltype(e)>);
    static_assert(ra::common_range<decltype(e)>);
    static_assert(ra::random_access_range<decltype(e)>);
    EXPECT_EQ(ra::size(e), xs.size());
    EXPECT_EQ(ra::size(vi::enumerate(vector<int>{})), 0u);

    // Common range: usable with iterator-pair constructors.
    const vector<std::tuple<ptrdiff_t, int>> v(e.begin(), e.end());
    EXPECT_EQ(
      v,
      (vector<std::tuple<ptrdiff_t, int>>{
        {0, 7},
        {1, 8},
        {2, 9}
    })
    );
    EXPECT_EQ(std::get<0>(*(e.end() - 1)), 2);

    // Unsized ranges still work.
    auto filtered = xs | vi::filter([](int x) {
                        return x != 8;
                    });
    auto ef = vi::enumerate(filtered);
    static_assert(!ra::sized_range<decltype(ef)>);
    vector<pair<ptrdiff_t, int>> result;
    for (auto [i, x] : ef) {
        result.emplace_back(i, x);
    }
    EXPECT_EQ(
      result,
      (vector<pair<ptrdiff_t, int>>{
        {0, 7},
        {1, 9}
    })
    );
}
