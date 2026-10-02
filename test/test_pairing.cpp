#include <curves.hpp>

using namespace rbp;

template <class C>
class PairingTest : public ::testing::Test{};

TYPED_TEST_SUITE(PairingTest, Curves);

TYPED_TEST(PairingTest, IsBilinearAndNonDegenerate){
    using C = TypeParam;
    const auto a = Zp<C>::random(), b = Zp<C>::random();
    const auto p = G1<C>::random(), p2 = G1<C>::random();
    const auto q = G2<C>::random();

    EXPECT_EQ(pair(G1<C>::generator(), G2<C>::generator()), Gt<C>::generator());
    EXPECT_FALSE(Gt<C>::generator().is_one());
    EXPECT_EQ(pair(p * a, q * b), pair(p, q).pow(a * b));
    EXPECT_EQ(pair(p + p2, q), pair(p, q) * pair(p2, q));
    EXPECT_TRUE(pair(G1<C>(), q).is_one());
}

TYPED_TEST(PairingTest, MultiPairingIsTheProductAcrossBatches){
    using C = TypeParam;
    std::vector<G1<C>> ps;
    std::vector<G2<C>> qs;
    Gt<C> product;
    for (int i = 0; i < 257; ++i){
        ps.push_back(G1<C>::random());
        qs.push_back(G2<C>::random());
        product *= pair(ps.back(), qs.back());
    }

    EXPECT_EQ(pair(ps, qs), product);
    EXPECT_TRUE(pair(std::vector<G1<C>>{}, std::vector<G2<C>>{}).is_one());
    qs.pop_back();
    EXPECT_THROW((void)pair(ps, qs), ShapeError);
}

TYPED_TEST(PairingTest, SymmetricCurvesPairTwoG1Points){
    using C = TypeParam;
    if constexpr (!C::symmetric){
        GTEST_SKIP() << "only type-1 curves pair G1 with G1";
    } else{
        const auto x = Zp<C>::random(), y = Zp<C>::random();
        EXPECT_EQ(pair(G1<C>::mul_generator(x), G1<C>::mul_generator(y)), Gt<C>::generator().pow(x * y));
    }
}
