#include <cstddef>
#include <cstdint>
#include <latch>
#include <optional>
#include <string>
#include <thread>
#include <vector>
#include <curves.hpp>

using namespace rbp;

namespace{
    constexpr std::size_t thread_count = 8;
    constexpr std::int64_t job_count = 16;

    template <class C>
    struct Shared{
        std::vector<G1<C>> ps{G1<C>::hash("shared", bytes_of("0")), G1<C>::hash("shared", bytes_of("1"))};
        std::vector<G2<C>> qs{G2<C>::hash("shared", bytes_of("0")), G2<C>::hash("shared", bytes_of("1"))};
        PreparedG2<C> prepared{qs};
        DlogTable<C> table{Gt<C>::generator(), 0, job_count};
        PairingProduct<C> product = [this]{
            PairingProduct<C> result;
            result.add(ps, qs);
            result.add(ps, prepared);
            return result;
        }();
    };

    template <class C>
    struct Outcome{
        Zp<C> scalar;
        G1<C> p;
        G2<C> q;
        Gt<C> single;
        Gt<C> product;
        Gt<C> shared_product;
        std::optional<std::int64_t> exponent;
        G1<C> p_decoded;
        G2<C> q_decoded;
        Gt<C> gt_decoded;

        bool operator==(const Outcome&) const = default;
    };

    template <class C>
    Outcome<C> run_job(const Shared<C>& shared, const std::int64_t job){
        const auto message = bytes_of(std::to_string(job));
        Outcome<C> outcome;
        outcome.scalar = Zp<C>::hash("concurrency", message);
        outcome.p = G1<C>::hash("concurrency", message);
        outcome.q = G2<C>::hash("concurrency", message);
        outcome.single = pair(outcome.p, outcome.q);
        PairingProduct<C> product;
        product.add(outcome.p, outcome.q);
        product.add({outcome.p * outcome.scalar, outcome.p}, shared.prepared);
        outcome.product = product.evaluate();
        outcome.shared_product = shared.product.evaluate();
        outcome.exponent = shared.table.find(Gt<C>::generator().pow(Zp<C>(job)));
        outcome.p_decoded = G1<C>::from_bytes(outcome.p.to_bytes());
        outcome.q_decoded = G2<C>::from_bytes(outcome.q.to_bytes());
        outcome.gt_decoded = Gt<C>::from_bytes(outcome.single.to_bytes());
        return outcome;
    }
}

template <class C>
class ConcurrencyTest : public ::testing::Test{};

TYPED_TEST_SUITE(ConcurrencyTest, Curves);

TYPED_TEST(ConcurrencyTest, ThreadsMatchTheSingleThreadedResults){
    using C = TypeParam;
    const Shared<C> shared;
    std::vector<Outcome<C>> expected, results(job_count);
    for (std::int64_t job = 0; job < job_count; ++job) expected.push_back(run_job(shared, job));

    std::latch start(thread_count);
    {
        std::vector<std::jthread> threads;
        for (std::size_t t = 0; t < thread_count; ++t){
            threads.emplace_back([&, t]{
                start.arrive_and_wait();
                for (auto job = static_cast<std::int64_t>(t); job < job_count; job += thread_count){
                    results[job] = run_job(shared, job);
                }
            });
        }
    }

    EXPECT_EQ(results, expected);
    for (std::int64_t job = 0; job < job_count; ++job) EXPECT_EQ(expected[job].exponent, job);
}
