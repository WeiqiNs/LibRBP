#include <algorithm>
#include <limits>
#include <curves.hpp>

using namespace rbp;

namespace{
    std::string decrement(std::string decimal){
        EXPECT_NE(decimal.back(), '0');
        --decimal.back();
        return decimal;
    }
}

template <class C>
class ZpTest : public ::testing::Test{};

TYPED_TEST_SUITE(ZpTest, Curves);

TYPED_TEST(ZpTest, ArithmeticAgreesWithIntegers){
    using Z = Zp<TypeParam>;
    EXPECT_EQ(Z(7) + Z(5), Z(12));
    EXPECT_EQ(Z(7) - Z(12), Z(-5));
    EXPECT_EQ(Z(6) * Z(7), Z(42));
    EXPECT_EQ(Z(42) / Z(7), Z(6));
    EXPECT_EQ(-Z(3), Z(-3));
    EXPECT_EQ(-Z(), Z());
    EXPECT_EQ(Z(3).pow(5), Z(243));
    EXPECT_EQ(Z(3).pow(0), Z(1));
    EXPECT_EQ(Z(5u), Z(5));

    Z x = 10;
    x += 4;
    x -= 2;
    x *= 3;
    x /= 4;
    EXPECT_EQ(x, Z(9));
}

TYPED_TEST(ZpTest, ValuesReduceModuloTheGroupOrder){
    using C = TypeParam;
    using Z = Zp<C>;
    const auto r_minus_one = decrement(std::string(Fixture<C>::order));

    EXPECT_EQ(Z(-1).to_string(), r_minus_one);
    EXPECT_TRUE((Z(-1) + 1).is_zero());
    EXPECT_FALSE(Z(1).is_zero());
    EXPECT_EQ(Z(std::numeric_limits<std::uint64_t>::max()).to_string(), "18446744073709551615");
    EXPECT_TRUE((Z(std::numeric_limits<std::int64_t>::min()) + Z(std::uint64_t{1} << 63)).is_zero());
}

TYPED_TEST(ZpTest, InverseExistsExceptForZero){
    using Z = Zp<TypeParam>;
    const auto x = Z::random();
    EXPECT_EQ(x * x.inverse(), Z(1));
    EXPECT_THROW((void)Z().inverse(), NotInvertible);
    EXPECT_THROW((void)(Z(1) / Z(0)), NotInvertible);
}

TYPED_TEST(ZpTest, EncodingIsFixedWidthBigEndianAndValidated){
    using Z = Zp<TypeParam>;
    const auto bytes = Z(258).to_bytes();
    ASSERT_EQ(bytes.size(), Z::byte_size());
    EXPECT_EQ(bytes.at(bytes.size() - 2), 0x01);
    EXPECT_EQ(bytes.back(), 0x02);
    EXPECT_TRUE(std::all_of(bytes.begin(), bytes.end() - 2, [](const auto b){ return b == 0; }));
    EXPECT_EQ(Z::from_bytes(bytes), Z(258));

    const auto largest = Z(-1).to_bytes();
    EXPECT_EQ(Z::from_bytes(largest), Z(-1));

    auto order = largest;
    ++order.back();
    EXPECT_THROW((void)Z::from_bytes(order), DecodeError);

    const Bytes short_by_one(largest.begin() + 1, largest.end());
    EXPECT_THROW((void)Z::from_bytes(short_by_one), DecodeError);
}

TYPED_TEST(ZpTest, HashIsDeterministicAndDomainSeparated){
    using Z = Zp<TypeParam>;
    const auto message = bytes_of("message");
    EXPECT_EQ(Z::hash("domain", message), Z::hash("domain", message));
    EXPECT_NE(Z::hash("domain", message), Z::hash("other", message));
    EXPECT_NE(Z::hash("domain", message), Z::hash("domain", bytes_of("massage")));
}
