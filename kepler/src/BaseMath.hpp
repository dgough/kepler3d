#pragma once

#include <glm/glm.hpp>

namespace kepler {
    extern const glm::mat4 IDENTITY_MATRIX;

    constexpr float PI = 3.14159265358979323846264338327950288f;
    constexpr float PI_OVER_2 = PI / 2.0f;
    constexpr float PI_OVER_4 = PI / 4.0f;

    glm::vec3 rgbToVec3(unsigned int color);
}
