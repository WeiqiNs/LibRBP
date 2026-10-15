#include <curves.hpp>

using namespace rbp;

template <class C>
class SymmetricTest : public ::testing::Test{};

TYPED_TEST_SUITE(SymmetricTest, SymmetricCurves);

TYPED_TEST(SymmetricTest, G1AndG2HashApartThoughTheyShareAGroup){
    using C = TypeParam;
    const auto message = bytes_of("message");
    EXPECT_EQ(G1<C>::generator().to_bytes(), G2<C>::generator().to_bytes());
    EXPECT_NE(G1<C>::hash("domain", message).to_bytes(), G2<C>::hash("domain", message).to_bytes());
}

TYPED_TEST(SymmetricTest, PairsTwoG1Points){
    using C = TypeParam;
    const auto x = Zp<C>::random(), y = Zp<C>::random();
    EXPECT_EQ(pair(G1<C>::mul_generator(x), G1<C>::mul_generator(y)), Gt<C>::generator().pow(x * y));
}
