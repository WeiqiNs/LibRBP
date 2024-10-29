#pragma once

#include "gmp.h"
#include "relic/relic.h"

class G1{
public:
    /// Hold a G1 group element.
    g1_t value;

    /// Default constructor.
    G1();

    /// Copy constructor.
    G1(const G1& other);

    /// Move constructor.
    G1(G1&& other) noexcept;

    /// Destructor.
    ~G1();

    /// Copy assignment operator.
    G1& operator=(const G1& other);

    /// Move assignment operator.
    G1& operator=(G1&& other) noexcept;

    /// Free a G1 group element.
    void free() const;
};

class G2{
public:
    /// Hold a G2 group element.
    g2_t value;

    /// Default constructor.
    G2();

    /// Copy constructor.
    G2(const G2& other);

    /// Move constructor.
    G2(G2&& other) noexcept;

    /// Destructor.
    ~G2();

    /// Copy assignment operator.
    G2& operator=(const G2& other);

    /// Move assignment operator.
    G2& operator=(G2&& other) noexcept;

    /// Free a G2 group element.
    void free() const;
};

class Gt{
public:
    /// Hold a Gt group element.
    gt_t value;

    /// Default constructor.
    Gt();

    /// Copy constructor.
    Gt(const Gt& other);

    /// Move constructor.
    Gt(Gt&& other) noexcept;

    /// Destructor.
    ~Gt();

    /// Copy assignment operator.
    Gt& operator=(const Gt& other);

    /// Move assignment operator.
    Gt& operator=(Gt&& other) noexcept;

    /// Free a Gt group element.
    void free() const;
};
