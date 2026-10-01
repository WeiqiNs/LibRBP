#include "relic.hpp"

namespace rbp{
    template <class C> requires C::symmetric
    Gt<C> pair(const G1<C>& p, const G1<C>& q){
        Gt<C> r;
        pc_map(raw(r), raw(p), raw(q));
        return r;
    }

    template Gt<detail::Tag> pair(const G1<detail::Tag>&, const G1<detail::Tag>&);
}
