#pragma once

#include <memory>
#include "group.hpp"

class BP{
public:
    /// Create instance for the field and group.
    std::unique_ptr<Field> Zp;
    std::unique_ptr<Group> Gp;

    /**
     * Default constructor for the bilinear pairing group class.
     * @param pre a boolean indicating whether using precompute tables for group exponentiation.
     */
    explicit BP(const bool& pre = true);

    /// Initialize the relic library core. This must be called prior using any functions in this library.
    static void init();

    /// Clear the relic library core.
    static void close();
};
