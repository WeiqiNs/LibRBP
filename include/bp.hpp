#pragma once

#include <memory>
#include "group.hpp"

class BP{
public:
    // Create instance for the field and group.
    std::unique_ptr<Field> Zp;
    std::unique_ptr<Group> Gp;

    explicit BP(const bool& pre = true);

    static void init();

    static void close();
};
