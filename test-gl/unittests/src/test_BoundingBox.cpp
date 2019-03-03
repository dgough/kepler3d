#include "common_test.hpp"

#include <BoundingBox.hpp>
#include <Transform.hpp>

using namespace kepler;
using glm::quat;
using glm::radians;

static const vec3 origin;

static constexpr float float_err = 0.000001f;

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

TEST(boundingBox, rotateY) {
    using std::sqrt;
    {
        BoundingBox actual(vec3(-1, -1, -1), vec3(1, 1, 1));
        Transform t;
        t.setRotationFromEuler(0, PI_OVER_4, 0);
        actual.transform(t.matrix());
        const float sqrt2 = sqrt(2.0f);
        EXPECT_VE3_EQ(vec3(-sqrt2, -1, -sqrt2), actual.min);
        EXPECT_VE3_EQ(vec3(sqrt2, 1, sqrt2), actual.max);
    }
    {
        BoundingBox actual(vec3(0), vec3(1, 1, 1));
        Transform t;
        t.setRotationFromEuler(0, -PI_OVER_4, 0);
        actual.transform(t.matrix());
        EXPECT_VE3_EQ(vec3(-sqrt(0.5f), 0.0f, 0), actual.min);
        EXPECT_VE3_EQ(vec3(sqrt(0.5f), 1, sqrt(2.0f)), actual.max);
    }
}

TEST(boundingBox, scaleTranslate) {
    const vec3 t(1, 2, 3);
    const vec3 s(10, 20, 30);
    BoundingBox actual(vec3(-1, -1, -1), vec3(1, 1, 1));
    BoundingBox expected;
    for (int i = 0; i < 3; ++i) {
        expected.min[i] = actual.min[i] * s[i] + t[i];
        expected.max[i] = actual.max[i] * s[i] + t[i];
    }
    Transform transform;
    transform.translate(t);
    transform.scale(s);
    actual.transform(transform.matrix());
    EXPECT_VE3_EQ(expected.min, actual.min);
    EXPECT_VE3_EQ(expected.max, actual.max);
}

TEST(boundingBox, rotateX) {
    BoundingBox actual(vec3(0, 0, 0), vec3(1, 1, 1));
    Transform t;
    t.setRotationFromEuler(-PI_OVER_2, 0, 0);
    actual.transform(t.matrix());
    BoundingBox expected(vec3(0, 0, -1), vec3(1, 1, 0));
    EXPECT_VE3_EQ(expected.min, actual.min);
    EXPECT_FLOAT_CLOSE(expected.max.x, actual.max.x, float_err);
    EXPECT_FLOAT_CLOSE(expected.max.y, actual.max.y, float_err);
    EXPECT_FLOAT_CLOSE(expected.max.z, actual.max.z, float_err);

}
