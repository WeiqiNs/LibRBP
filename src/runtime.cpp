#include "relic.hpp"

namespace rbp{
    namespace detail{
        template <class C>
        Runtime<C>::Runtime(){
            core_init();
            if (relic_failed()) throw RelicError("RELIC core_init failed");
            if (pc_param_set_any() != RLC_OK || relic_failed()) throw RelicError("RELIC has no pairing curve");
            bn_new(order_);
            pc_get_ord(order_);
            order_bytes_ = bn_size_bin(order_);
        }

        template <class C>
        const Runtime<C>& Runtime<C>::require(){
            static const Runtime runtime;
            return runtime;
        }

        template class Runtime<Tag>;
    }

    template <class C>
    void seed(const ByteView bytes){
        detail::Runtime<C>::require();
        if (bytes.empty()) throw ShapeError("seed needs at least one byte");
        Bytes copy(bytes.begin(), bytes.end());
        core_get()->seeded = 0;
        rand_seed(copy.data(), copy.size());
    }

    template void seed<detail::Tag>(ByteView);
}
