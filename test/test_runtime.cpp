#include <thread>
#include <curves.hpp>

using namespace rbp;

template <class C>
class RuntimeTest : public ::testing::Test{};

TYPED_TEST_SUITE(RuntimeTest, Curves);

TYPED_TEST(RuntimeTest, SeedMakesTheRandomSequenceReproducible){
    using C = TypeParam;
    seed<C>(bytes_of("seed-a"));
    const std::vector first{Zp<C>::random(), Zp<C>::random()};
    seed<C>(bytes_of("seed-a"));
    const std::vector again{Zp<C>::random(), Zp<C>::random()};
    seed<C>(bytes_of("seed-b"));
    const std::vector other{Zp<C>::random(), Zp<C>::random()};

    EXPECT_EQ(first, again);
    EXPECT_NE(first, other);
    EXPECT_NE(first.front(), first.back());
    EXPECT_THROW(seed<C>(Bytes{}), ShapeError);
}

TYPED_TEST(RuntimeTest, SeedReachesOnlyTheCallingThread){
    using C = TypeParam;
    seed<C>(bytes_of("seed-a"));
    const auto first = Zp<C>::random();
    seed<C>(bytes_of("seed-a"));

    Zp<C> unseeded, other_unseeded, reseeded;
    std::thread([&]{
        unseeded = Zp<C>::random();
        seed<C>(bytes_of("seed-a"));
        reseeded = Zp<C>::random();
    }).join();
    std::thread([&]{ other_unseeded = Zp<C>::random(); }).join();
    const auto next = Zp<C>::random();

    EXPECT_NE(unseeded, first);
    EXPECT_NE(unseeded, other_unseeded);
    EXPECT_EQ(reseeded, first);
    EXPECT_EQ(next, first);
}

TYPED_TEST(RuntimeTest, ThreadLocalDestructorsCanStillUseTheCurve){
    using C = TypeParam;
    struct PairsOnExit{
        Gt<C>& result;

        ~PairsOnExit(){ result = pair(G1<C>::generator(), G2<C>::generator()); }
    };
    Gt<C> result;
    std::thread([&]{
        thread_local const PairsOnExit pairs_on_exit{result};
        (void)Zp<C>::random();
    }).join();

    EXPECT_EQ(result, pair(G1<C>::generator(), G2<C>::generator()));
}
