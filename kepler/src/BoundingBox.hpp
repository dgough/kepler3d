#pragma once

#include <BaseMath.hpp>

namespace kepler {

class BoundingBox final {
public:
    BoundingBox() {}
    BoundingBox(const vec3& min, const vec3& max) : min(min), max(max) {}
    ~BoundingBox() = default;
    BoundingBox(const BoundingBox&) = default;
    BoundingBox& operator=(const BoundingBox&) = default;

    void set(const vec3& min, const vec3& max) {
        this->min = min;
        this->max = max;
    }

    /// Returns true if this bounding box is empty.
    bool empty() const;
    /// Returns the center 3D position.
    vec3 center() const;
    void corners(vec3* p) const;

    void merge(const BoundingBox& box);

    void transform(const mat4& matrix);

    vec3 min;
    vec3 max;
};
}
