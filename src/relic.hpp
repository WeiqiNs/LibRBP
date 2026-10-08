#ifndef RBP_DETAIL_RELIC_HPP
#define RBP_DETAIL_RELIC_HPP

#include <new>
#include <type_traits>
#include <curve.hpp>
#include <gmp.h>
#include <pthread.h>
#include <relic.h>

#ifdef CHECK
#error "LibRBP requires RELIC built with CHECK off: CHECK unwinds with longjmp, which skips C++ destructors"
#endif

#if !defined(MULTI) || MULTI != PTHREAD
#error "LibRBP requires RELIC built with MULTI=PTHREAD, so that every thread has its own RELIC context"
#endif

static_assert(ALLOC == AUTO, "LibRBP stores RELIC elements inline and requires ALLOC=AUTO");
static_assert(sizeof(dig_t) == sizeof(std::uint64_t), "LibRBP requires a 64-bit RELIC build");

namespace rbp::detail{
    using G1Element = std::remove_extent_t<g1_t>;
    using G2Element = std::remove_extent_t<g2_t>;
    using GtElement = std::remove_extent_t<gt_t>;

    static_assert(sizeof(bn_st) == Tag::zp_size && alignof(bn_st) <= 16);
    static_assert(sizeof(g1_t) == Tag::g1_size && alignof(G1Element) <= 16);
    static_assert(sizeof(g2_t) == Tag::g2_size && alignof(G2Element) <= 16);
    static_assert(sizeof(gt_t) == Tag::gt_size && alignof(GtElement) <= 16);
    static_assert(Tag::symmetric == static_cast<bool>(pc_map_is_type1()));

    template <class T>
    struct RelicElement;

    template <class C>
    struct RelicElement<Zp<C>>{ using type = bn_st; };

    template <class C>
    struct RelicElement<G1<C>>{ using type = G1Element; };

    template <class C>
    struct RelicElement<G2<C>>{ using type = G2Element; };

    template <class C>
    struct RelicElement<Gt<C>>{ using type = GtElement; };

    struct Raw{
        template <class T>
        static auto of(T& x){
            using Element = typename RelicElement<std::remove_const_t<T>>::type;
            using Pointer = std::conditional_t<std::is_const_v<T>, const Element*, Element*>;
            return std::launder(reinterpret_cast<Pointer>(x.storage_));
        }

        template <class C>
        static const std::vector<G2<C>>& points(const PreparedG2<C>& prepared){ return prepared.points_; }

        template <class C>
        static const std::vector<std::uint64_t>& lines(const PreparedG2<C>& prepared){ return prepared.lines_; }
    };

    template <class C>
    class Runtime{
    public:
        static const Runtime& require();

        [[nodiscard]] const bn_st* order() const{ return order_; }
        [[nodiscard]] std::size_t order_bytes() const{ return order_bytes_; }

    private:
        Runtime();

        bn_t order_;
        std::size_t order_bytes_;
        pthread_key_t thread_contexts_;
    };
}

namespace rbp::detail{
    inline constexpr std::size_t batch_size = 256;

    template <class T>
    auto raw(T& x){ return Raw::of(x); }

    inline void clear_relic_error(){ err_get_code(); }

    inline bool relic_failed(){ return err_get_code() != RLC_OK; }
}

#endif
