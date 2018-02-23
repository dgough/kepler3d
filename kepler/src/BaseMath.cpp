#include "stdafx.h"
#include "BaseMath.hpp"

namespace kepler {
const glm::mat4 IDENTITY_MATRIX;

glm::vec3 rgbToVec3(unsigned int color) {
    float r = static_cast<float>(color >> 16 & 0xFF) / 255.0f;
    float g = static_cast<float>(color >> 8 & 0xFF) / 255.0f;
    float b = static_cast<float>(color & 0xFF) / 255.0f;
    return glm::vec3(r, g, b);
}
}
