#ifndef RBP_LINALG_HPP
#define RBP_LINALG_HPP

#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include "core.hpp"

namespace rbp{
    template <class C>
    using Vector = std::vector<Zp<C>>;

    namespace detail{
        template <class C, class F>
        [[nodiscard]] Vector<C> elementwise(const Vector<C>& x, const Vector<C>& y, const char* operation, const F f){
            if (x.size() != y.size()) throw ShapeError(std::string(operation) + " needs vectors of equal length");
            Vector<C> r;
            r.reserve(x.size());
            for (std::size_t i = 0; i < x.size(); ++i) r.push_back(f(x[i], y[i]));
            return r;
        }
    }

    template <class C>
    [[nodiscard]] Vector<C> random_vector(const std::size_t size){
        Vector<C> r;
        r.reserve(size);
        for (std::size_t i = 0; i < size; ++i) r.push_back(Zp<C>::random());
        return r;
    }

    template <class C>
    [[nodiscard]] Vector<C> operator+(const Vector<C>& x, const Vector<C>& y){
        return detail::elementwise(x, y, "vector addition", std::plus<>{});
    }

    template <class C>
    [[nodiscard]] Vector<C> operator-(const Vector<C>& x, const Vector<C>& y){
        return detail::elementwise(x, y, "vector subtraction", std::minus<>{});
    }

    template <class C>
    [[nodiscard]] Vector<C> operator*(const Vector<C>& x, const std::type_identity_t<Zp<C>>& k){
        Vector<C> r;
        r.reserve(x.size());
        for (const auto& v : x) r.push_back(v * k);
        return r;
    }

    template <class C>
    [[nodiscard]] Vector<C> operator*(const std::type_identity_t<Zp<C>>& k, const Vector<C>& x){
        return x * k;
    }

    template <class C>
    [[nodiscard]] Vector<C> hadamard(const Vector<C>& x, const Vector<C>& y){
        return detail::elementwise(x, y, "hadamard", std::multiplies<>{});
    }

    template <class C>
    [[nodiscard]] Zp<C> sum(const Vector<C>& x){
        Zp<C> total;
        for (const auto& v : x) total += v;
        return total;
    }

    template <class C>
    [[nodiscard]] Zp<C> inner(const Vector<C>& x, const Vector<C>& y){
        return sum(hadamard(x, y));
    }

    template <class C>
    [[nodiscard]] Vector<C> concat(Vector<C> x, const Vector<C>& y){
        x.insert(x.end(), y.begin(), y.end());
        return x;
    }

    template <class C>
    [[nodiscard]] Vector<C> poly_from_roots(const Vector<C>& roots, const std::size_t degree){
        if (roots.size() > degree) throw ShapeError("poly_from_roots needs degree >= number of roots");
        Vector<C> coefficients(degree + 1);
        coefficients.front() = 1;
        for (std::size_t n = 0; n < roots.size(); ++n){
            for (std::size_t i = n + 1; i > 0; --i) coefficients[i] = coefficients[i - 1] - roots[n] * coefficients[i];
            coefficients.front() = -roots[n] * coefficients.front();
        }
        return coefficients;
    }

    template <class C>
    class Matrix;

    template <class C>
    struct Inversion{
        Matrix<C> inverse;
        Zp<C> determinant;
    };

    template <class C>
    class Matrix{
    public:
        Matrix(const std::size_t rows, const std::size_t cols) : rows_(rows), cols_(cols), data_(rows * cols){
            if (rows == 0 || cols == 0) throw ShapeError("a matrix needs at least one row and one column");
        }

        [[nodiscard]] static Matrix from_rows(const std::vector<Vector<C>>& rows){
            Matrix m(rows.size(), rows.empty() ? 0 : rows.front().size());
            for (std::size_t i = 0; i < m.rows_; ++i){
                if (rows[i].size() != m.cols_) throw ShapeError("matrix rows must all have the same length");
                for (std::size_t j = 0; j < m.cols_; ++j) m.at(i, j) = rows[i][j];
            }
            return m;
        }

        [[nodiscard]] static Matrix identity(const std::size_t size){
            Matrix m(size, size);
            for (std::size_t i = 0; i < size; ++i) m.at(i, i) = 1;
            return m;
        }

        [[nodiscard]] static Matrix random(const std::size_t rows, const std::size_t cols){
            Matrix m(rows, cols);
            for (auto& v : m.data_) v = Zp<C>::random();
            return m;
        }

        [[nodiscard]] std::size_t rows() const{ return rows_; }
        [[nodiscard]] std::size_t cols() const{ return cols_; }

        Zp<C>& at(const std::size_t i, const std::size_t j){ return data_[index(i, j)]; }
        [[nodiscard]] const Zp<C>& at(const std::size_t i, const std::size_t j) const{ return data_[index(i, j)]; }

        [[nodiscard]] Matrix transpose() const{
            Matrix t(cols_, rows_);
            for (std::size_t i = 0; i < rows_; ++i) for (std::size_t j = 0; j < cols_; ++j) t.at(j, i) = at(i, j);
            return t;
        }

        [[nodiscard]] bool is_identity() const{
            return rows_ == cols_ && *this == identity(rows_);
        }

        [[nodiscard]] Zp<C> determinant() const{
            return eliminate().first;
        }

        [[nodiscard]] Matrix inverse() const{
            return inverse_with_determinant().inverse;
        }

        [[nodiscard]] Inversion<C> inverse_with_determinant() const{
            auto [determinant, inverse] = eliminate();
            if (!inverse) throw NotInvertible("a singular matrix has no inverse");
            return {std::move(*inverse), determinant};
        }

        friend bool operator==(const Matrix& a, const Matrix& b) = default;

        friend Matrix operator*(const Matrix& a, const Matrix& b){
            if (a.cols_ != b.rows_) throw ShapeError("matrix product needs left columns equal to right rows");
            Matrix r(a.rows_, b.cols_);
            for (std::size_t i = 0; i < a.rows_; ++i)
                for (std::size_t j = 0; j < b.cols_; ++j)
                    for (std::size_t k = 0; k < a.cols_; ++k) r.at(i, j) += a.at(i, k) * b.at(k, j);
            return r;
        }

        friend Vector<C> operator*(const Matrix& a, const Vector<C>& x){
            if (a.cols_ != x.size()) throw ShapeError("matrix-vector product needs one entry per column");
            Vector<C> r(a.rows_);
            for (std::size_t i = 0; i < a.rows_; ++i)
                for (std::size_t k = 0; k < a.cols_; ++k) r[i] += a.at(i, k) * x[k];
            return r;
        }

        friend Vector<C> operator*(const Vector<C>& x, const Matrix& a){
            return a.transpose() * x;
        }

        friend Matrix operator*(const Matrix& a, const Zp<C>& k){
            Matrix r = a;
            for (auto& v : r.data_) v *= k;
            return r;
        }

        friend Matrix operator*(const Zp<C>& k, const Matrix& a){
            return a * k;
        }

        friend Matrix hcat(const Matrix& a, const Matrix& b){
            if (a.rows_ != b.rows_) throw ShapeError("hcat needs matrices with the same number of rows");
            Matrix r(a.rows_, a.cols_ + b.cols_);
            for (std::size_t i = 0; i < a.rows_; ++i){
                for (std::size_t j = 0; j < a.cols_; ++j) r.at(i, j) = a.at(i, j);
                for (std::size_t j = 0; j < b.cols_; ++j) r.at(i, a.cols_ + j) = b.at(i, j);
            }
            return r;
        }

    private:
        [[nodiscard]] std::size_t index(const std::size_t i, const std::size_t j) const{
            if (i >= rows_ || j >= cols_) throw std::out_of_range("matrix index outside its rows or columns");
            return i * cols_ + j;
        }

        void swap_rows(const std::size_t a, const std::size_t b){
            for (std::size_t j = 0; j < cols_; ++j) std::swap(at(a, j), at(b, j));
        }

        [[nodiscard]] std::pair<Zp<C>, std::optional<Matrix>> eliminate() const{
            if (rows_ != cols_) throw ShapeError("only square matrices have a determinant or an inverse");
            const auto n = rows_;
            Matrix work = hcat(*this, identity(n));
            Zp<C> determinant = 1;
            for (std::size_t col = 0; col < n; ++col){
                auto pivot = col;
                while (pivot < n && work.at(pivot, col).is_zero()) ++pivot;
                if (pivot == n) return {Zp<C>(), std::nullopt};
                if (pivot != col){
                    work.swap_rows(pivot, col);
                    determinant = -determinant;
                }
                determinant *= work.at(col, col);
                const auto scale = work.at(col, col).inverse();
                for (std::size_t j = col; j < 2 * n; ++j) work.at(col, j) *= scale;
                for (std::size_t r = 0; r < n; ++r){
                    if (r == col || work.at(r, col).is_zero()) continue;
                    const auto factor = work.at(r, col);
                    for (std::size_t j = col; j < 2 * n; ++j) work.at(r, j) -= factor * work.at(col, j);
                }
            }
            Matrix inverse(n, n);
            for (std::size_t i = 0; i < n; ++i)
                for (std::size_t j = 0; j < n; ++j) inverse.at(i, j) = work.at(i, n + j);
            return {determinant, std::move(inverse)};
        }

        std::size_t rows_;
        std::size_t cols_;
        Vector<C> data_;
    };
}

#endif
