#ifndef RBP_IO_HPP
#define RBP_IO_HPP

#include <format>
#include <ostream>
#include <string>
#include "core.hpp"

namespace rbp{
    [[nodiscard]] inline std::string to_hex(const ByteView bytes){
        constexpr std::string_view digits = "0123456789abcdef";
        std::string out;
        out.reserve(2 * bytes.size());
        for (const auto byte : bytes){
            out.push_back(digits[byte >> 4]);
            out.push_back(digits[byte & 0x0f]);
        }
        return out;
    }

    template <class C>
    std::ostream& operator<<(std::ostream& out, const Zp<C>& x){
        return out << x.to_string();
    }

    template <class C, Side S>
    std::ostream& operator<<(std::ostream& out, const Point<C, S>& p){
        return out << to_hex(p.to_bytes());
    }

    template <class C>
    std::ostream& operator<<(std::ostream& out, const Gt<C>& x){
        return out << to_hex(x.to_bytes());
    }
}

template <class C>
struct std::formatter<rbp::Zp<C>> : std::formatter<std::string>{
    auto format(const rbp::Zp<C>& x, std::format_context& context) const{
        return std::formatter<std::string>::format(x.to_string(), context);
    }
};

template <class C, rbp::Side S>
struct std::formatter<rbp::Point<C, S>> : std::formatter<std::string>{
    auto format(const rbp::Point<C, S>& p, std::format_context& context) const{
        return std::formatter<std::string>::format(rbp::to_hex(p.to_bytes()), context);
    }
};

template <class C>
struct std::formatter<rbp::Gt<C>> : std::formatter<std::string>{
    auto format(const rbp::Gt<C>& x, std::format_context& context) const{
        return std::formatter<std::string>::format(rbp::to_hex(x.to_bytes()), context);
    }
};

#endif
