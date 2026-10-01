#pragma once

#include <stdexcept>

namespace rbp{
    class Error : public std::runtime_error{
    public:
        using std::runtime_error::runtime_error;
    };

    class RelicError : public Error{
    public:
        using Error::Error;
    };

    class DecodeError : public Error{
    public:
        using Error::Error;
    };

    class NotInvertible : public Error{
    public:
        using Error::Error;
    };

    class ShapeError : public Error{
    public:
        using Error::Error;
    };
}
