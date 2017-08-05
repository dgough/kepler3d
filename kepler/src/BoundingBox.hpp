#pragma once

#include <glm/glm.hpp>

namespace kepler {

    class BoundingBox final {
    public:
        BoundingBox() {}
        BoundingBox(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}
        ~BoundingBox() = default;
        BoundingBox(const BoundingBox&) = default;
        BoundingBox& operator=(const BoundingBox&) = default;

        void set(const glm::vec3& min, const glm::vec3& max) {
            this->min = min;
            this->max = max;
        }
        
        /// Returns true if this bounding box is empty.
        bool empty() const;
        /// Returns the center 3D position.
        glm::vec3 center() const;


        void merge(const BoundingBox& box);

        glm::vec3 min;
        glm::vec3 max;
    };
}
