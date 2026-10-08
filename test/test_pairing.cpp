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

TYPED_TEST(PairingTest, PairingProductMixesPreparedAndPlainPairs){
    using C = TypeParam;
    Zp<C> exponent;
    const auto random_pairs = [&]{
        std::pair<std::vector<G1<C>>, std::vector<G2<C>>> pairs;
        for (int i = 0; i < 7; ++i){
            const auto a = Zp<C>::random(), b = Zp<C>::random();
            exponent += a * b;
            pairs.first.push_back(G1<C>::mul_generator(a));
            pairs.second.push_back(G2<C>::mul_generator(b));
        }
        return pairs;
    };
    const auto [plain_ps, plain_qs] = random_pairs();
    const auto [first_ps, first_qs] = random_pairs();
    auto [second_ps, second_qs] = random_pairs();
    second_ps.push_back(G1<C>::random());
    second_qs.push_back(G2<C>());
    const PreparedG2 first(first_qs), second(second_qs);

    PairingProduct<C> product;
    product.add(plain_ps, plain_qs);
    product.add(G1<C>::random(), G2<C>());
    product.add(first_ps, first);
    product.add(second_ps, second);
    EXPECT_EQ(product.evaluate(), Gt<C>::generator().pow(exponent));
}

TYPED_TEST(PairingTest, PreparedPairingMatchesTheMultiPairing){
    using C = TypeParam;
    std::vector<G2<C>> qs;
    for (int i = 0; i < 7; ++i) qs.push_back(G2<C>::random());
    const PreparedG2 prepared(qs);
    for (int round = 0; round < 3; ++round){
        std::vector<G1<C>> ps;
        for (int i = 0; i < 7; ++i) ps.push_back(G1<C>::random());
        EXPECT_EQ(pair(ps, prepared), pair(ps, qs));
    }

    EXPECT_EQ(pair(std::vector{G1<C>::generator()}, PreparedG2(std::vector{G2<C>::generator()})), Gt<C>::generator());
    EXPECT_TRUE(pair(std::vector<G1<C>>{}, PreparedG2(std::vector<G2<C>>{})).is_one());
}

TYPED_TEST(PairingTest, PairingProductRejectsTemporaryPreparedKeys){
    using C = TypeParam;
    constexpr bool keeps_a_temporary = requires(
        PairingProduct<C> product, std::vector<G1<C>> ps, std::vector<G2<C>> qs
    ){
        product.add(ps, PreparedG2<C>(qs));
    };
    static_assert(!keeps_a_temporary);
}

TYPED_TEST(PairingTest, PreparedPairingSkipsIdentitiesAndChecksShape){
    using C = TypeParam;
    std::vector<G1<C>> ps;
    std::vector<G2<C>> qs;
    for (int i = 0; i < 4; ++i){
        ps.push_back(G1<C>::random());
        qs.push_back(G2<C>::random());
    }
    qs[0] = G2<C>();
    ps[2] = G1<C>();

    const PreparedG2 prepared(qs);
    EXPECT_EQ(pair(ps, prepared), pair(ps, qs));
    EXPECT_TRUE(pair(ps, PreparedG2(std::vector<G2<C>>(4))).is_one());
    EXPECT_THROW((void)pair(std::vector(ps.begin(), ps.begin() + 3), prepared), ShapeError);
    ps.push_back(G1<C>::random());
    EXPECT_THROW((void)pair(ps, prepared), ShapeError);
}
