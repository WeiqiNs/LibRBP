#include <exception>
#include "relic.hpp"

namespace rbp{
    namespace detail{
        namespace{
            bool set_up_relic(){
                core_init();
                return pc_param_set_any() == RLC_OK && !relic_failed();
            }

            void release_context(void* context){
                core_set(nullptr);
                delete static_cast<ctx_t*>(context);
            }

            [[noreturn]] void abandon_setup(const char* reason){
                core_set(nullptr);
                throw RelicError(reason);
            }

            void initialize_thread(void* thread_contexts) noexcept{
                auto* context = new ctx_t{};
                pthread_setspecific(*static_cast<const pthread_key_t*>(thread_contexts), context);
                core_set(context);
                if (!set_up_relic()) std::terminate();
            }
        }

        template <class C>
        Runtime<C>::Runtime(){
            if (!set_up_relic()) abandon_setup("RELIC could not set up its pairing curve");
            if (pthread_key_create(&thread_contexts_, release_context) != 0){
                abandon_setup("could not create the key that owns each thread's RELIC context");
            }
            core_set_thread_initializer(initialize_thread, &thread_contexts_);
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
