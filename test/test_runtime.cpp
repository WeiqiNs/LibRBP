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
