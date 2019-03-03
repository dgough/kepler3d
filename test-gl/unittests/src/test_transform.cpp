#include "common_test.hpp"

#include <Node.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace kepler;
using glm::quat;
using glm::radians;

TEST(transform, transform_identity) {
    Transform t;
    EXPECT_EQ(t.translation(), vec3());
    EXPECT_EQ(t.rotation(), quat());
    EXPECT_EQ(t.scale(), vec3(1));
    EXPECT_EQ(t.matrix(), mat4());
}

TEST(transform, transform_set) {
    Transform t;

    vec3 translation(645, 54, 37);
    quat rotation(vec3(1.0f, 0.5f, 0.0f));
    vec3 scale(1, 2, 3);
    t.set(translation, rotation, scale);
    EXPECT_EQ(t.translation(), translation);
    EXPECT_EQ(t.rotation(), rotation);
    EXPECT_EQ(t.scale(), scale);
}

TEST(transform, combine_with_parent) {
    Transform parent;
    parent.scale(2.0f);
    vec3 translation(2.0f, 0.5f, -1.0f);
    Transform child(translation);

    child.combineWithParent(parent);
    EXPECT_EQ(child.translation(), translation * 2.0f);
}

TEST(transform, euler) {
    {
        vec3 euler = vec3(radians(45.0f), 0.0f, 0.0f);
        quat rot(euler);
        EXPECT_FLOAT_EQ(euler.x, pitch(rot));
    }
    {
        vec3 euler = vec3(0.0f, radians(45.0f), 0.0f);
        quat rot(euler);
        EXPECT_FLOAT_EQ(euler.y, yaw(rot));
    }
    {
        vec3 euler = vec3(0.0f, 0.0f, radians(45.0f));
        quat rot(euler);
        EXPECT_FLOAT_EQ(euler.z, roll(rot));
    }
}

TEST(transform, rotate_vector) {
    EXPECT_EQ(sizeof(vec3), sizeof(float) * 3);
}

TEST(math, pointers) {
    mat4 matrix;
    vec3 position(1, 2, 3);

    matrix = glm::translate(matrix, position);

    float f[16] = { 0 };
    memcpy(f, glm::value_ptr(matrix), sizeof(float) * 16);
    EXPECT_FLOAT_EQ(1, 1);
}

TEST(transform, translate) {
    Transform t;
    EXPECT_FLOAT_EQ(t.translation().x, 0);
    EXPECT_FLOAT_EQ(t.translation().y, 0);
    EXPECT_FLOAT_EQ(t.translation().z, 0);
    vec3 a(1, 2, 3);
    t.translate(a.x, a.y, a.z);
    EXPECT_VE3_EQ(t.translation(), a);
    t.translate(a.x, a.y, a.z);
    EXPECT_VE3_EQ(t.translation(), a * 2.0f);

    t.setTranslation(10, 20, 30);
    EXPECT_VE3_EQ(t.translation(), vec3(10, 20, 30));
    t.translate(-5, -10, -15);
    EXPECT_VE3_EQ(t.translation(), vec3(5, 10, 15));
    t.translate(a);
    EXPECT_FLOAT_EQ(t.translation().x, 5 + a.x);
    EXPECT_FLOAT_EQ(t.translation().y, 10 + a.y);
    EXPECT_FLOAT_EQ(t.translation().z, 15 + a.z);
    t.setTranslation(a);
    EXPECT_VE3_EQ(t.translation(), a);
}

TEST(transform, scale) {
    Transform t;
    EXPECT_VE3_EQ(t.scale(), vec3(1, 1, 1));
    vec3 s(45.324f, -365.26f, 1234.0f);
    t.setScale(s.x, s.y, s.z);
    EXPECT_VE3_EQ(t.scale(), s);
    t.scale(-10.0f);
    EXPECT_VE3_EQ(t.scale(), s * -10.0f);
    t.setScale(s);
    EXPECT_VE3_EQ(t.scale(), s);
}

TEST(transform, equals) {
    Transform t1;
    Transform t2;

    EXPECT_EQ(t1, t2);
    EXPECT_FALSE(t1 != t2);
    t2.scale(2.f);
    EXPECT_NE(t1, t2);
    t2.setScale(vec3(1, 1, 1));
    EXPECT_EQ(t1, t2);
}
