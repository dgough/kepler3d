#pragma once

#include <glm/glm.hpp>

namespace kepler {

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

extern const mat4 IDENTITY_MATRIX;

constexpr float PI = 3.14159265358979323846264338327950288f;
constexpr float PI_OVER_2 = PI / 2.0f;
constexpr float PI_OVER_4 = PI / 4.0f;

} // namespace kepler
