#include "stdafx.h"
#include "ColorMath.hpp"

namespace kepler {

vec3 rgbToVec3(unsigned int rgbColor) {
    return vec3(rgbColor >> 16 & 0xFF, rgbColor >> 8 & 0xFF, rgbColor & 0xFF) / 255.0f;
}

} // namespace kepler
