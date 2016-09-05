#include "gtest/gtest.h"
#include "macros.hpp"

#include <Rectangle.hpp>

using namespace kepler;
using glm::vec2;

TEST(rectangle, create) {
    Rectangle rect(1, 2, 3, 4);
    EXPECT_EQ(rect.x(), 1.f);
    EXPECT_EQ(rect.y(), 2.f);
    EXPECT_EQ(rect.width(), 3.f);
    EXPECT_EQ(rect.height(), 4.f);

    Rectangle rect2(rect);
    EXPECT_EQ(rect, rect2);
    EXPECT_FALSE(rect != rect2);

    Rectangle rect3(10, 20, 30, 40);
    rect2 = rect3;
    EXPECT_EQ(rect2, rect3);

}

TEST(rectangle, contains) {
    static constexpr float x = 20.f;
    static constexpr float y = 30.f;
    static constexpr float width = 100.f;
    static constexpr float height = 40.f;
    Rectangle rect(x, y, width, height);

    EXPECT_FALSE(rect.contains(vec2()));
    EXPECT_FALSE(rect.contains(vec2(x, y - 1.f)));
    EXPECT_FALSE(rect.contains(vec2(x - 0.1f, y)));
    EXPECT_FALSE(rect.contains(vec2(x + width + 0.001f, y + height)));

    EXPECT_TRUE(rect.contains(vec2(x, y)));
    EXPECT_TRUE(rect.contains(vec2(x + width, y + height)));
}
