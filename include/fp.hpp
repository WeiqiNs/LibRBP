#pragma once

#include <string>
#include <vector>
#include "gmp.h"
#include "relic/relic.h"

class Fp{
public:
    /// Object to hold the actual point value.
    bn_t value;

    /// Default constructor.
    Fp();

    /// Constructor with input integer x.
    explicit Fp(int x);

    /// Constructor with input string x, base 10, say "123".
    explicit Fp(const std::string& x);

    /// Constructor with input unsigned hex characters, say "0x12", this is in "big endian" format.
    explicit Fp(const std::vector<unsigned char>& x);

    /// Copy constructor.
    Fp(const Fp& other);

    /// Move constructor.
    Fp(Fp&& other) noexcept;

    /// Destructor.
    ~Fp();

    /// Copy assignment operator.
    Fp& operator=(const Fp& other);

    /// Move assignment operator.
    Fp& operator=(Fp&& other) noexcept;

    /// Clear the point object.
    void free() const;
};
