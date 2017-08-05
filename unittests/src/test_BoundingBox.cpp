#include "gtest/gtest.h"
#include "macros.hpp"

#include <BoundingBox.hpp>

using namespace kepler;
using glm::vec3;
using glm::quat;
using glm::mat4;
using glm::radians;

static const vec3 origin;

TEST(boundingBox, empty) {
    EXPECT_TRUE(BoundingBox().empty());
    EXPECT_TRUE(BoundingBox(vec3(5), vec3(5)).empty());

    BoundingBox a(vec3(-1), vec3(1));
    EXPECT_FALSE(a.empty());
    a.set(vec3(1), vec3(2));
    EXPECT_FALSE(a.empty());
}

TEST(boundingBox, center) {    
    EXPECT_VE3_EQ(origin, BoundingBox().center());
    BoundingBox a(vec3(-1, -1, -1), vec3(1, 1, 1));
    EXPECT_VE3_EQ(origin, a.center());
    BoundingBox b(vec3(5, 10, 15), vec3(15, 30, 65));
    EXPECT_VE3_EQ(vec3(10, 20, 40), b.center());
}

TEST(boundingBox, merge) {
    {
        BoundingBox a(vec3(1, 2, 3), vec3(10, 20, 30));
        BoundingBox b(vec3(-5, -10, 7), vec3(5, 100, 3));
        a.merge(b);
        EXPECT_VE3_EQ(vec3(-5, -10, 3), a.min);
        EXPECT_VE3_EQ(vec3(10, 100, 30), a.max);
    }
}