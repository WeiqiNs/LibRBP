#include "curves.hpp"

using namespace rbp;

template <class C>
class LinalgTest : public ::testing::Test{
protected:
    using M = Matrix<C>;
    using V = Vector<C>;

    static M matrix(const std::vector<std::vector<int>>& rows){
        std::vector<V> converted;
        for (const auto& row : rows) converted.emplace_back(row.begin(), row.end());
        return M::from_rows(converted);
    }

    static V vector(const std::vector<int>& values){
        return V(values.begin(), values.end());
    }
};

TYPED_TEST_SUITE(LinalgTest, Curves);

TYPED_TEST(LinalgTest, VectorOperationsAreElementwise){
    using Z = Zp<TypeParam>;
    const auto x = TestFixture::vector({1, 2, 3});
    const auto y = TestFixture::vector({4, 5, 6});

    EXPECT_EQ(x + y, TestFixture::vector({5, 7, 9}));
    EXPECT_EQ(x - y, TestFixture::vector({-3, -3, -3}));
    EXPECT_EQ(x * Z(2), TestFixture::vector({2, 4, 6}));
    EXPECT_EQ(Z(2) * x, TestFixture::vector({2, 4, 6}));
    EXPECT_EQ(hadamard(x, y), TestFixture::vector({4, 10, 18}));
    EXPECT_EQ(inner(x, y), Z(32));
    EXPECT_EQ(sum(x), Z(6));
    EXPECT_EQ(concat(x, y), TestFixture::vector({1, 2, 3, 4, 5, 6}));
    EXPECT_EQ(random_vector<TypeParam>(4).size(), 4u);

    EXPECT_THROW((void)(x + TestFixture::vector({1, 2})), ShapeError);
}

TYPED_TEST(LinalgTest, PolyFromRootsExpandsTheProduct){
    using V = typename TestFixture::V;
    EXPECT_EQ(poly_from_roots(TestFixture::vector({1, 2}), 2), TestFixture::vector({2, -3, 1}));
    EXPECT_EQ(poly_from_roots(TestFixture::vector({1, 2}), 4), TestFixture::vector({2, -3, 1, 0, 0}));
    EXPECT_EQ(poly_from_roots(TestFixture::vector({0}), 1), TestFixture::vector({0, 1}));
    EXPECT_EQ(poly_from_roots(V{}, 2), TestFixture::vector({1, 0, 0}));
    EXPECT_THROW((void)poly_from_roots(TestFixture::vector({1, 2, 3}), 2), ShapeError);
}

TYPED_TEST(LinalgTest, ProductsTransposeAndConcatenation){
    using Z = Zp<TypeParam>;
    const auto a = TestFixture::matrix({{1, 2}, {3, 4}});

    EXPECT_EQ(a * TestFixture::matrix({{0, 1}, {1, 0}}), TestFixture::matrix({{2, 1}, {4, 3}}));
    EXPECT_EQ(a * TestFixture::vector({5, 6}), TestFixture::vector({17, 39}));
    EXPECT_EQ(TestFixture::vector({5, 6}) * a, TestFixture::vector({23, 34}));
    EXPECT_EQ(a * Z(2), TestFixture::matrix({{2, 4}, {6, 8}}));
    EXPECT_EQ(Z(3) * a, TestFixture::matrix({{3, 6}, {9, 12}}));
    EXPECT_EQ(a.transpose(), TestFixture::matrix({{1, 3}, {2, 4}}));
    EXPECT_EQ(hcat(a, TestFixture::matrix({{5}, {6}})), TestFixture::matrix({{1, 2, 5}, {3, 4, 6}}));
    EXPECT_EQ(a.rows(), 2u);
    EXPECT_EQ(a.cols(), 2u);
}

TYPED_TEST(LinalgTest, InverseAndDeterminantUseRowSwaps){
    using Z = Zp<TypeParam>;
    using M = typename TestFixture::M;
    const auto swap = TestFixture::matrix({{0, 1}, {1, 0}});
    const auto [swap_inverse, swap_det] = swap.inverse_with_determinant();
    EXPECT_EQ(swap_inverse, swap);
    EXPECT_EQ(swap_det, Z(-1));

    EXPECT_EQ(TestFixture::matrix({{2, 1}, {1, 1}}).inverse(), TestFixture::matrix({{1, -1}, {-1, 2}}));
    const auto three = TestFixture::matrix({{3, 5, 8}, {2, 2, 2}, {9, 9, 3}});
    EXPECT_EQ(three.determinant(), Z(24));
    EXPECT_TRUE((three * three.inverse()).is_identity());

    const auto random = M::random(5, 5);
    EXPECT_TRUE((random * random.inverse()).is_identity());
}

TYPED_TEST(LinalgTest, SingularMatricesHaveDeterminantZeroAndNoInverse){
    const auto singular = TestFixture::matrix({{1, 2}, {2, 4}});
    EXPECT_TRUE(singular.determinant().is_zero());
    EXPECT_THROW((void)singular.inverse(), NotInvertible);
}

TYPED_TEST(LinalgTest, IdentityIsRecognizedOnlyWhenSquareAndExact){
    using M = typename TestFixture::M;
    EXPECT_TRUE(M::identity(3).is_identity());
    EXPECT_FALSE(TestFixture::matrix({{1, 0}, {0, 2}}).is_identity());
    EXPECT_FALSE(TestFixture::matrix({{1, 0}}).is_identity());
}

TYPED_TEST(LinalgTest, ShapesAreValidated){
    using M = typename TestFixture::M;
    using V = typename TestFixture::V;
    const auto wide = TestFixture::matrix({{1, 2, 3}, {4, 5, 6}});

    EXPECT_THROW(M(0, 2), ShapeError);
    EXPECT_THROW(M(2, 0), ShapeError);
    EXPECT_THROW((void)M::from_rows({V(2), V(3)}), ShapeError);
    EXPECT_THROW((void)(wide * wide), ShapeError);
    EXPECT_THROW((void)(wide * TestFixture::vector({1, 2})), ShapeError);
    EXPECT_THROW((void)hcat(wide, M(3, 1)), ShapeError);
    EXPECT_THROW((void)wide.determinant(), ShapeError);
    EXPECT_THROW((void)wide.at(2, 0), std::out_of_range);
    EXPECT_THROW((void)wide.at(0, 3), std::out_of_range);
}
