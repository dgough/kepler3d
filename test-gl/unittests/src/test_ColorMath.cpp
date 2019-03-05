#include "common_test.hpp"
#include <ColorMath.hpp>

using namespace kepler;

TEST(color_math, rgbToVec3) {
    EXPECT_VE3_EQ(vec3(), rgbToVec3(0));
    EXPECT_VE3_EQ(vec3(1, 0, 0), rgbToVec3(0xFF0000));
    EXPECT_VE3_EQ(vec3(0, 1, 0), rgbToVec3(0x00FF00));
    EXPECT_VE3_EQ(vec3(0, 0, 1), rgbToVec3(0x0000FF));
    EXPECT_VE3_EQ(vec3(1), rgbToVec3(0xFFFFFF));
    EXPECT_VE3_EQ(vec3(1, 106.0f/255.0f, 0), rgbToVec3(0xFF6A00));
}
