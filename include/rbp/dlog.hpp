#ifndef RBP_DLOG_HPP
#define RBP_DLOG_HPP

#include <algorithm>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>
#include "core.hpp"

namespace rbp{
    namespace detail{
        inline std::uint64_t dlog_span(const std::int64_t lo, const std::int64_t hi){
            if (lo > hi) throw ShapeError("dlog needs lo <= hi");
            return static_cast<std::uint64_t>(hi) - static_cast<std::uint64_t>(lo);
        }

        inline std::uint64_t baby_step_count(const std::uint64_t span){
            std::uint64_t root = 0;
            for (std::uint64_t bit = std::uint64_t{1} << 31; bit != 0; bit >>= 1){
                const auto candidate = root | bit;
                if (candidate <= span / candidate) root = candidate;
            }
            return root + 1;
        }

        inline std::int64_t offset(const std::int64_t lo, const std::uint64_t k){
            return static_cast<std::int64_t>(static_cast<std::uint64_t>(lo) + k);
        }
    }

    template <class C>
    class DlogTable{
    public:
        DlogTable(const Gt<C>& base, const std::int64_t lo, const std::int64_t hi)
            : base_(base), lo_(lo), span_(detail::dlog_span(lo, hi)), steps_(detail::baby_step_count(span_)),
              shift_(base.pow(-Zp<C>(lo))), giant_(base.pow(-Zp<C>(steps_))){
            if (base.is_one()) return;
            baby_.reserve(steps_);
            Gt<C> power;
            for (std::uint64_t j = 0; j < steps_; ++j){
                baby_.emplace_back(fingerprint(power), j);
                power *= base;
            }
            std::ranges::sort(baby_);
        }

        [[nodiscard]] std::optional<std::int64_t> find(const Gt<C>& target) const{
            if (base_.is_one()) return target.is_one() ? std::optional(lo_) : std::nullopt;
            auto gamma = target * shift_;
            for (std::uint64_t i = 0; i <= span_ / steps_; ++i){
                const auto matches = std::ranges::equal_range(baby_, fingerprint(gamma), {}, &Entry::first);
                for (const auto& entry : matches){
                    const auto k = i * steps_ + entry.second;
                    if (k <= span_ && base_.pow(Zp<C>(detail::offset(lo_, k))) == target) return detail::offset(lo_, k);
                }
                gamma *= giant_;
            }
            return std::nullopt;
        }

    private:
        using Entry = std::pair<std::uint64_t, std::uint64_t>;

        static std::uint64_t fingerprint(const Gt<C>& x){
            const auto bytes = x.to_bytes();
            std::uint64_t hash = 1469598103934665603ull;
            for (const auto byte : std::span(bytes).first(std::min<std::size_t>(32, bytes.size())))
                hash = (hash ^ byte) * 1099511628211ull;
            return hash;
        }

        Gt<C> base_;
        std::int64_t lo_;
        std::uint64_t span_;
        std::uint64_t steps_;
        Gt<C> shift_;
        Gt<C> giant_;
        std::vector<Entry> baby_;
    };

    template <class C>
    [[nodiscard]] std::optional<std::int64_t> dlog(
        const Gt<C>& base, const Gt<C>& target, const std::int64_t lo, const std::int64_t hi
    ){
        return DlogTable<C>(base, lo, hi).find(target);
    }
}

#endif
