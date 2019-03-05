#pragma once

#include <BaseMath.hpp>

namespace kepler {


/// Converts an RGB integer color into vec3.
/// For example, 0xFF6A00 would be orange.
vec3 rgbToVec3(unsigned int rgbColor);

} // namespace kepler
