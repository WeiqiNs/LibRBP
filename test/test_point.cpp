#include "curves.hpp"

using namespace rbp;

template <class P>
class PointTest : public ::testing::Test{
protected:
    using Curve = typename PointTraits<P>::Curve;
    using Z = Zp<Curve>;
};

TYPED_TEST_SUITE(PointTest, Points);

TYPED_TEST(PointTest, GroupLawsHold){
    using P = TypeParam;
    using Z = typename TestFixture::Z;
    const auto p = P::random(), q = P::random();

    EXPECT_EQ(p + P(), p);
    EXPECT_TRUE((p - p).is_identity());
    EXPECT_TRUE((-p + p).is_identity());
    EXPECT_EQ(p + q, q + p);
    EXPECT_EQ(p * Z(2), p + p);
    EXPECT_EQ(Z(3) * p, p + p + p);
    EXPECT_FALSE(p.is_identity());

    auto acc = p;
    acc += q;
    acc -= p;
    acc *= 2;
    EXPECT_EQ(acc, q + q);
}

TYPED_TEST(PointTest, GeneratorHasTheGroupOrder){
    using P = TypeParam;
    using Z = typename TestFixture::Z;
    const auto x = Z::random();

    EXPECT_EQ(P::mul_generator(x), P::generator() * x);
    EXPECT_TRUE((P::generator() * Z(-1) + P::generator()).is_identity());
    EXPECT_TRUE(P::mul_generator(Z()).is_identity());
}

TYPED_TEST(PointTest, EveryEncodingRoundTrips){
    using P = TypeParam;
    const auto p = P::random();

    EXPECT_EQ(P::from_bytes(p.to_bytes()), p);
    EXPECT_EQ(P::from_bytes(p.to_bytes(Encoding::uncompressed)), p);
    EXPECT_EQ(p.to_bytes().size(), PointTraits<P>::fixture.compressed_size);
    EXPECT_GT(p.to_bytes(Encoding::uncompressed).size(), p.to_bytes().size());
    EXPECT_EQ(P().to_bytes(), Bytes{0});
    EXPECT_TRUE(P::from_bytes(P().to_bytes()).is_identity());
}

TYPED_TEST(PointTest, DecodingRejectsInvalidEncodings){
    using P = TypeParam;
    const auto valid = P::generator().to_bytes();
    ASSERT_EQ(P::from_bytes(valid), P::generator());

    if (const auto off_subgroup = PointTraits<P>::fixture.off_subgroup()){
        EXPECT_THROW((void)P::from_bytes(*off_subgroup), DecodeError);
    }
    EXPECT_THROW((void)P::from_bytes(ByteView(valid).first(valid.size() - 1)), DecodeError);
    EXPECT_THROW((void)P::from_bytes(Bytes{1}), DecodeError);
    auto bad_prefix = valid;
    bad_prefix.front() = 0x05;
    EXPECT_THROW((void)P::from_bytes(bad_prefix), DecodeError);
}

TYPED_TEST(PointTest, HashLandsInTheSubgroupAndSeparatesDomains){
    using P = TypeParam;
    const auto message = to_bytes("message");
    const auto h = P::hash("domain", message);

    EXPECT_EQ(P::from_bytes(h.to_bytes()), h);
    EXPECT_EQ(P::hash("domain", message), h);
    EXPECT_NE(P::hash("other", message), h);
    EXPECT_NE(P::hash("domain", to_bytes("massage")), h);
}

TYPED_TEST(PointTest, MsmMatchesTheNaiveSumAcrossBatches){
    using P = TypeParam;
    using Z = typename TestFixture::Z;
    std::vector<P> points;
    std::vector<Z> scalars;
    std::vector<P> terms;
    for (int i = 0; i < 257; ++i){
        points.push_back(P::random());
        scalars.push_back(Z::random());
        terms.push_back(points.back() * scalars.back());
    }

    EXPECT_EQ(msm(points, scalars), sum(terms));
    EXPECT_TRUE(msm(std::vector<P>{}, std::vector<Z>{}).is_identity());
    EXPECT_TRUE(sum(std::vector<P>{}).is_identity());
    scalars.pop_back();
    EXPECT_THROW((void)msm(points, scalars), ShapeError);
}

template <class C>
class SideTest : public ::testing::Test{};

TYPED_TEST_SUITE(SideTest, Curves);

TYPED_TEST(SideTest, G1AndG2HashApartEvenOnSymmetricCurves){
    using C = TypeParam;
    if constexpr (!C::symmetric){
        GTEST_SKIP() << "G1 and G2 are different groups on asymmetric curves";
    } else{
        const auto message = to_bytes("message");
        EXPECT_EQ(G1<C>::generator().to_bytes(), G2<C>::generator().to_bytes());
        EXPECT_NE(G1<C>::hash("domain", message).to_bytes(), G2<C>::hash("domain", message).to_bytes());
    }
}
