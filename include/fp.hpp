#pragma once

#include "gmp.h"
#include "relic/relic.h"

class Fp{
public:
    /// Object to hold the actual point value.
    bn_t value;

    /// Default constructor.
    Fp();

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
