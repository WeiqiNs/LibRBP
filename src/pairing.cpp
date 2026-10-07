#include "relic.hpp"

namespace rbp{
    using detail::raw;

    namespace{
        template <class C, class Q>
        Gt<C> product_of(const std::vector<G1<C>>& ps, const Q& qs){
            PairingProduct<C> product;
            product.add(ps, qs);
            return product.evaluate();
        }
    }

    template <class C>
    Gt<C> pair(const G1<C>& p, const G2<C>& q){
        Gt<C> r;
        pc_map(raw(r), raw(p), raw(q));
        return r;
    }

    template <class C>
    Gt<C> pair(const std::vector<G1<C>>& ps, const std::vector<G2<C>>& qs){
        return product_of(ps, qs);
    }

    template <class C>
    Gt<C> pair(const std::vector<G1<C>>& ps, const PreparedG2<C>& qs){
        return product_of(ps, qs);
    }

    template Gt<detail::Tag> pair(const G1<detail::Tag>&, const G2<detail::Tag>&);
    template Gt<detail::Tag> pair(const std::vector<G1<detail::Tag>>&, const std::vector<G2<detail::Tag>>&);
    template Gt<detail::Tag> pair(const std::vector<G1<detail::Tag>>&, const PreparedG2<detail::Tag>&);
}
