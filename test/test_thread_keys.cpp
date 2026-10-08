#include <thread>
#include <vector>
#include <pthread.h>
#include <curves.hpp>

using namespace rbp;

namespace{
    std::vector<pthread_key_t> exhaust_thread_keys(){
        std::vector<pthread_key_t> keys;
        for (pthread_key_t key; pthread_key_create(&key, nullptr) == 0;) keys.push_back(key);
        return keys;
    }
}

template <class C>
class ThreadKeyTest : public ::testing::Test{};

TYPED_TEST_SUITE(ThreadKeyTest, Curves);

TYPED_TEST(ThreadKeyTest, FailedFirstUseLeavesTheThreadOutOfTheContextAnotherThreadSetsUp){
    using C = TypeParam;
    const auto keys = exhaust_thread_keys();
    EXPECT_THROW((void)Zp<C>(), RelicError);
    for (const auto key : keys) pthread_key_delete(key);

    std::thread([]{
        seed<C>(bytes_of("shared"));
        (void)Zp<C>::random();
    }).join();
    const auto drawn_here = Zp<C>::random();
    seed<C>(bytes_of("shared"));
    (void)Zp<C>::random();

    EXPECT_NE(drawn_here, Zp<C>::random());
}
