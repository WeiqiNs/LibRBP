#include <curves.hpp>

using namespace rbp;

TEST(MultiCurveTest, CurvesKeepIndependentState){
    using A = std::tuple_element_t<0, CurveTuple>;
    using B = std::tuple_element_t<1, CurveTuple>;
    const auto a = Zp<A>::random(), b = Zp<A>::random();
    const auto before = pair(G1<A>::mul_generator(a), G2<A>::mul_generator(b));

    const auto x = Zp<B>::random(), y = Zp<B>::random();
    EXPECT_EQ(pair(G1<B>::mul_generator(x), G2<B>::mul_generator(y)), Gt<B>::generator().pow(x * y));

    EXPECT_EQ(pair(G1<A>::mul_generator(a), G2<A>::mul_generator(b)), before);
    EXPECT_EQ(before, Gt<A>::generator().pow(a * b));
}
