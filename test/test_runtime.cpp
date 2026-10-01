#include "curves.hpp"

using namespace rbp;

template <class C>
class RuntimeTest : public ::testing::Test{};

TYPED_TEST_SUITE(RuntimeTest, Curves);

TYPED_TEST(RuntimeTest, SeedMakesTheRandomSequenceReproducible){
    using C = TypeParam;
    seed<C>(to_bytes("seed-a"));
    const std::vector first{Zp<C>::random(), Zp<C>::random()};
    seed<C>(to_bytes("seed-a"));
    const std::vector again{Zp<C>::random(), Zp<C>::random()};
    seed<C>(to_bytes("seed-b"));
    const std::vector other{Zp<C>::random(), Zp<C>::random()};

    EXPECT_EQ(first, again);
    EXPECT_NE(first, other);
    EXPECT_NE(first.front(), first.back());
    EXPECT_THROW(seed<C>(Bytes{}), ShapeError);
}

TEST(RuntimeTest, CurvesKeepIndependentState){
    if constexpr (std::tuple_size_v<CurveTuple> < 2){
        GTEST_SKIP() << "needs two curves";
    } else{
        using A = std::tuple_element_t<0, CurveTuple>;
        using B = std::tuple_element_t<1, CurveTuple>;
        const auto a = Zp<A>::random(), b = Zp<A>::random();
        const auto before = pair(G1<A>::mul_generator(a), G2<A>::mul_generator(b));

        const auto x = Zp<B>::random(), y = Zp<B>::random();
        EXPECT_EQ(pair(G1<B>::mul_generator(x), G2<B>::mul_generator(y)), Gt<B>::generator().pow(x * y));

        EXPECT_EQ(pair(G1<A>::mul_generator(a), G2<A>::mul_generator(b)), before);
        EXPECT_EQ(before, Gt<A>::generator().pow(a * b));
    }
}
