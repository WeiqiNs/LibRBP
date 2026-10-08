#ifndef RBP_CORE_HPP
#define RBP_CORE_HPP

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "errors.hpp"

#define RBP_API __attribute__((visibility("default")))

namespace rbp{
    using Bytes = std::vector<std::uint8_t>;
    using ByteView = std::span<const std::uint8_t>;

    enum class Side{ g1, g2 };

    enum class Encoding{ compressed, uncompressed };

    namespace detail{
        struct Raw;

        template <class I>
        concept Character = std::same_as<I, char> || std::same_as<I, wchar_t> || std::same_as<I, char8_t>
            || std::same_as<I, char16_t> || std::same_as<I, char32_t>;
    }

    template <class C>
    class RBP_API Zp{
    public:
        Zp();

        template <std::signed_integral I> requires (!detail::Character<I> && sizeof(I) <= sizeof(std::int64_t))
        Zp(const I value) : Zp(from_signed(value)){}

        template <std::unsigned_integral I>
            requires (!std::same_as<I, bool> && !detail::Character<I> && sizeof(I) <= sizeof(std::uint64_t))
        Zp(const I value) : Zp(from_unsigned(value)){}

        [[nodiscard]] static Zp random();
        [[nodiscard]] static Zp hash(std::string_view domain, ByteView message);
        [[nodiscard]] static Zp from_bytes(ByteView bytes);
        [[nodiscard]] static std::size_t byte_size();

        [[nodiscard]] Bytes to_bytes() const;
        [[nodiscard]] std::string to_string() const;
        [[nodiscard]] bool is_zero() const;
        [[nodiscard]] Zp inverse() const;
        [[nodiscard]] Zp pow(std::uint64_t exponent) const;

        friend Zp operator+(const Zp& x, const Zp& y){ return x.plus(y); }
        friend Zp operator-(const Zp& x, const Zp& y){ return x.minus(y); }
        friend Zp operator*(const Zp& x, const Zp& y){ return x.times(y); }
        friend Zp operator/(const Zp& x, const Zp& y){ return x.times(y.inverse()); }
        friend Zp operator-(const Zp& x){ return x.negated(); }
        friend bool operator==(const Zp& x, const Zp& y){ return x.equals(y); }

        Zp& operator+=(const Zp& y){ return *this = *this + y; }
        Zp& operator-=(const Zp& y){ return *this = *this - y; }
        Zp& operator*=(const Zp& y){ return *this = *this * y; }
        Zp& operator/=(const Zp& y){ return *this = *this / y; }

    private:
        friend struct detail::Raw;

        static Zp from_signed(std::int64_t value);
        static Zp from_unsigned(std::uint64_t value);

        [[nodiscard]] Zp plus(const Zp& y) const;
        [[nodiscard]] Zp minus(const Zp& y) const;
        [[nodiscard]] Zp times(const Zp& y) const;
        [[nodiscard]] Zp negated() const;
        [[nodiscard]] bool equals(const Zp& y) const;

        alignas(16) std::byte storage_[C::zp_size];
    };

    template <class C, Side S>
    class RBP_API Point{
    public:
        Point();

        [[nodiscard]] static Point generator();
        [[nodiscard]] static Point random();
        [[nodiscard]] static Point hash(std::string_view domain, ByteView message);
        [[nodiscard]] static Point mul_generator(const Zp<C>& scalar);

        template <std::same_as<std::vector<Zp<C>>> V>
        [[nodiscard]] static std::vector<Point> mul_generator(const V& scalars){
            std::vector<Point> points;
            points.reserve(scalars.size());
            for (const auto& scalar : scalars) points.push_back(mul_generator(scalar));
            return points;
        }
        [[nodiscard]] static Point from_bytes(ByteView bytes);

        [[nodiscard]] Bytes to_bytes(Encoding encoding = Encoding::compressed) const;
        [[nodiscard]] bool is_identity() const;

        friend Point operator+(const Point& p, const Point& q){ return p.plus(q); }
        friend Point operator-(const Point& p, const Point& q){ return p.plus(q.negated()); }
        friend Point operator-(const Point& p){ return p.negated(); }
        friend Point operator*(const Point& p, const Zp<C>& k){ return p.times(k); }
        friend Point operator*(const Zp<C>& k, const Point& p){ return p.times(k); }
        friend bool operator==(const Point& p, const Point& q){ return p.equals(q); }

        Point& operator+=(const Point& q){ return *this = *this + q; }
        Point& operator-=(const Point& q){ return *this = *this - q; }
        Point& operator*=(const Zp<C>& k){ return *this = *this * k; }

    private:
        friend struct detail::Raw;

        [[nodiscard]] Point plus(const Point& q) const;
        [[nodiscard]] Point times(const Zp<C>& k) const;
        [[nodiscard]] Point negated() const;
        [[nodiscard]] bool equals(const Point& q) const;

        alignas(16) std::byte storage_[S == Side::g1 ? C::g1_size : C::g2_size];
    };

    template <class C>
    using G1 = Point<C, Side::g1>;

    template <class C>
    using G2 = Point<C, Side::g2>;

    template <class C>
    class RBP_API Gt{
    public:
        Gt();

        [[nodiscard]] static Gt generator();
        [[nodiscard]] static Gt random();
        [[nodiscard]] static Gt from_bytes(ByteView bytes);

        [[nodiscard]] Bytes to_bytes() const;
        [[nodiscard]] bool is_one() const;
        [[nodiscard]] std::uint64_t fingerprint() const;
        [[nodiscard]] Gt inverse() const;
        [[nodiscard]] Gt pow(const Zp<C>& exponent) const;

        friend Gt operator*(const Gt& x, const Gt& y){ return x.times(y); }
        friend Gt operator/(const Gt& x, const Gt& y){ return x.times(y.inverse()); }
        friend bool operator==(const Gt& x, const Gt& y){ return x.equals(y); }

        Gt& operator*=(const Gt& y){ return *this = *this * y; }
        Gt& operator/=(const Gt& y){ return *this = *this / y; }

    private:
        friend struct detail::Raw;

        [[nodiscard]] Gt times(const Gt& y) const;
        [[nodiscard]] bool equals(const Gt& y) const;

        alignas(16) std::byte storage_[C::gt_size];
    };

    template <class C>
    class RBP_API PreparedG2{
    public:
        explicit PreparedG2(std::vector<G2<C>> points);

    private:
        friend struct detail::Raw;

        std::vector<G2<C>> points_;
        std::vector<std::uint64_t> lines_;
    };

    template <class C>
    class RBP_API PairingProduct{
    public:
        void add(const G1<C>& p, const G2<C>& q);
        void add(const std::vector<G1<C>>& ps, const std::vector<G2<C>>& qs);
        void add(const std::vector<G1<C>>& ps, const PreparedG2<C>& qs);
        void add(const std::vector<G1<C>>& ps, const PreparedG2<C>&& qs) = delete;

        [[nodiscard]] Gt<C> evaluate() const;

    private:
        struct PreparedTerm{
            std::vector<G1<C>> ps;
            const PreparedG2<C>* qs;
        };

        std::vector<G1<C>> ps_;
        std::vector<G2<C>> qs_;
        std::vector<PreparedTerm> prepared_;
    };

    template <class C>
    [[nodiscard]] RBP_API Gt<C> pair(const G1<C>& p, const G2<C>& q);

    template <class C>
    [[nodiscard]] RBP_API Gt<C> pair(const std::vector<G1<C>>& ps, const std::vector<G2<C>>& qs);

    template <class C>
    [[nodiscard]] RBP_API Gt<C> pair(const std::vector<G1<C>>& ps, const PreparedG2<C>& qs);

    template <class C> requires C::symmetric
    [[nodiscard]] RBP_API Gt<C> pair(const G1<C>& p, const G1<C>& q);

    template <class C, Side S>
    [[nodiscard]] RBP_API Point<C, S> msm(const std::vector<Point<C, S>>& points, const std::vector<Zp<C>>& scalars);

    template <class C>
    RBP_API void seed(ByteView bytes);

    template <class C, Side S>
    [[nodiscard]] Point<C, S> sum(const std::vector<Point<C, S>>& points){
        Point<C, S> total;
        for (const auto& p : points) total += p;
        return total;
    }

    [[nodiscard]] inline Bytes bytes_of(const std::string_view text){
        return {text.begin(), text.end()};
    }
}

#endif
