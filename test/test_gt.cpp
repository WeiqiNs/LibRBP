#include <limits>
#include "curves.hpp"

using namespace rbp;

template <class C>
class GtTest : public ::testing::Test{};

TYPED_TEST_SUITE(GtTest, Curves);

TYPED_TEST(GtTest, GroupLawsHold){
    using T = Gt<TypeParam>;
    const auto x = T::random(), y = T::random();

    EXPECT_EQ(x * T(), x);
    EXPECT_TRUE((x / x).is_one());
    EXPECT_TRUE((x * x.inverse()).is_one());
    EXPECT_EQ(x * y, y * x);
    EXPECT_FALSE(x.is_one());

    auto acc = x;
    acc *= y;
    acc /= x;
    EXPECT_EQ(acc, y);
}

TYPED_TEST(GtTest, PowAcceptsNegativeExponents){
    using T = Gt<TypeParam>;
    const auto g = T::generator();

    EXPECT_EQ(g.pow(-1), g.inverse());
    EXPECT_EQ(g.pow(3), g * g * g);
    EXPECT_TRUE(g.pow(0).is_one());
}

TYPED_TEST(GtTest, EncodingRoundTripsIncludingOne){
    using T = Gt<TypeParam>;
    const auto x = T::random();

    EXPECT_EQ(T::from_bytes(x.to_bytes()), x);
    EXPECT_TRUE(T::from_bytes(T().to_bytes()).is_one());
}

TYPED_TEST(GtTest, DecodingRejectsInvalidEncodings){
    using T = Gt<TypeParam>;
    const auto valid = T::generator().to_bytes();
    ASSERT_EQ(T::from_bytes(valid), T::generator());

    auto outside = valid;
    outside.back() ^= 0x01;
    EXPECT_THROW((void)T::from_bytes(outside), DecodeError);
    EXPECT_THROW((void)T::from_bytes(Bytes(valid.size(), 0xFF)), DecodeError);
    EXPECT_THROW((void)T::from_bytes(ByteView(valid).first(valid.size() - 1)), DecodeError);
}

TYPED_TEST(GtTest, DlogFindsEveryExponentInItsInclusiveRange){
    using T = Gt<TypeParam>;
    const auto g = T::generator();
    const DlogTable<TypeParam> table(g, -20, 20);
    for (std::int64_t k = -20; k <= 20; ++k) EXPECT_EQ(table.find(g.pow(k)), k);
    EXPECT_EQ(table.find(g.pow(21)), std::nullopt);
    EXPECT_EQ(table.find(g.pow(-21)), std::nullopt);
    EXPECT_EQ(dlog(g, g.pow(777), -1000, 1000), 777);
    EXPECT_THROW((void)dlog(g, g, 1, 0), ShapeError);
}

TYPED_TEST(GtTest, DlogHandlesExtremeRangesAndTheIdentityBase){
    using T = Gt<TypeParam>;
    constexpr auto min = std::numeric_limits<std::int64_t>::min();
    constexpr auto max = std::numeric_limits<std::int64_t>::max();
    const auto g = T::generator();

    EXPECT_EQ(dlog(g, g.pow(max), max - 1, max), max);
    EXPECT_EQ(dlog(g, T(), max, max), std::nullopt);
    EXPECT_EQ(dlog(g, g.pow(min), min, min + 100), min);

    const DlogTable<TypeParam> trivial(T(), 5, 100);
    EXPECT_EQ(trivial.find(T()), 5);
    EXPECT_EQ(trivial.find(g), std::nullopt);
}
