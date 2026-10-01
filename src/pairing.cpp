#include <algorithm>
#include <memory>
#include "relic.hpp"

namespace rbp{
    template <class C>
    Gt<C> pair(const G1<C>& p, const G2<C>& q){
        Gt<C> r;
        pc_map(raw(r), raw(p), raw(q));
        return r;
    }

    template <class C>
    Gt<C> pair(const std::vector<G1<C>>& ps, const std::vector<G2<C>>& qs){
        if (ps.size() != qs.size()) throw ShapeError("multi-pairing needs one G2 point per G1 point");
        Gt<C> total;
        for (std::size_t start = 0; start < ps.size(); start += batch_size){
            const auto count = std::min(batch_size, ps.size() - start);
            const auto lefts = std::make_unique<g1_t[]>(count);
            const auto rights = std::make_unique<g2_t[]>(count);
            for (std::size_t i = 0; i < count; ++i){
                g1_copy(lefts[i], raw(ps[start + i]));
                g2_copy(rights[i], raw(qs[start + i]));
            }
            Gt<C> part;
            pc_map_sim(raw(part), lefts.get(), rights.get(), count);
            total *= part;
        }
        return total;
    }

    template Gt<detail::Tag> pair(const G1<detail::Tag>&, const G2<detail::Tag>&);
    template Gt<detail::Tag> pair(const std::vector<G1<detail::Tag>>&, const std::vector<G2<detail::Tag>>&);
}
