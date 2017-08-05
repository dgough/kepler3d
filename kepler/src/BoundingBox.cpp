#include "stdafx.h"
#include "BoundingBox.hpp"

namespace kepler {
    bool BoundingBox::empty() const {
        return min == max;
    }
    glm::vec3 BoundingBox::center() const {
        return (max - min) * 0.5f + min;
    }
    void BoundingBox::merge(const BoundingBox& box) {
        min = glm::min(min, box.min);
        max = glm::max(max, box.max);
    }
}
