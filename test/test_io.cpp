#include <format>
#include <sstream>
#include "curves.hpp"

using namespace rbp;

TEST(IoTest, HexIsTwoLowercaseDigitsPerByte){
    EXPECT_EQ(to_hex(Bytes{0x00, 0xab, 0x10, 0xff}), "00ab10ff");
    EXPECT_EQ(to_hex(Bytes{}), "");
}

template <class C>
class FormatTest : public ::testing::Test{};

TYPED_TEST_SUITE(FormatTest, Curves);

TYPED_TEST(FormatTest, ScalarsPrintInDecimal){
    using Z = Zp<TypeParam>;
    EXPECT_EQ(std::format("{}", Z(42)), "42");
    EXPECT_EQ(std::format("{:>4}", Z(7)), "   7");

    std::ostringstream out;
    out << Z(1234);
    EXPECT_EQ(out.str(), "1234");
}

TYPED_TEST(FormatTest, GroupElementsPrintTheirEncodingInHex){
    using C = TypeParam;
    EXPECT_EQ(std::format("{}", G1<C>()), "00");
    EXPECT_EQ(std::format("{}", G2<C>()), "00");

    const auto generator = std::format("{}", G1<C>::generator());
    EXPECT_EQ(generator.size(), 2 * Fixture<C>::g1.compressed_size);
    EXPECT_TRUE(generator.starts_with("02") || generator.starts_with("03"));

    const auto g = Gt<C>::generator();
    const auto gt = std::format("{}", g);
    EXPECT_EQ(gt.size(), 2 * g.to_bytes().size());

    std::ostringstream out;
    out << G1<C>() << ' ' << G2<C>::generator() << ' ' << g;
    EXPECT_EQ(out.str(), "00 " + std::format("{}", G2<C>::generator()) + " " + gt);
}
