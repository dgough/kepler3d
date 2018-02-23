#pragma once

#include "BoundingBox.hpp"

namespace kepler {

class Bounded {
public:
    virtual ~Bounded() = default;
    virtual bool getBoundingBox(BoundingBox& box) = 0;
};
}
