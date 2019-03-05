#include "common_test.hpp"

#include <Node.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace kepler;
using glm::quat;
using glm::radians;

constexpr float FLOAT_ERR = 0.000001f;

TEST(node_transform, parent_transform) {
    auto root = Node::create("root");
    auto a = root->createChild("A");
    auto b = a->createChild("B");
    auto c = b->createChild("C");

    const Transform& t = root->localTransform();
    float x = t.translation().x;

    vec3 translation(1.0, 2.0, 3.0);
    root->editLocalTransform().translate(translation);
    EXPECT_EQ(c->worldTransform().translation(), root->localTransform().translation());
    EXPECT_EQ(c->worldTransform().translation(), translation);
    //root->editLocalTransform().rotate(glm::quat(vec3(0.0f, glm::radians(45.0f), 0.0f)));
}

TEST(node_transform, forward) {
    auto a = Node::create("a");
    auto b = a->createChild("b");

    a->translate(vec3(86, 1234, -43));
    //a->scale(5.0f);

    vec3 forward(0, 0, -1);
    EXPECT_EQ(b->forwardVectorWorld(), forward);
    {
        a->rotate(quat(vec3(0, radians(45.0f), 0)));
        vec3 normalized = normalize(vec3(-1, 0, -1));
        vec3 f = normalize(b->forwardVectorWorld());
        EXPECT_VE3_EQ(f, normalized);
    }

    {
        a->editLocalTransform().loadIdentity();
        a->rotate(quat(vec3(radians(-45.0f), radians(45.0f), 0.0f)));
        rotate(quat(), vec3());
        vec3 normalized = normalize(vec3(-1.0f, -sqrtf(2.0f), -1.0f));
        vec3 f = normalize(b->forwardVectorWorld());
        EXPECT_VE3_EQ(f, normalized);
    }
}

TEST(node_transform, rotate_forward_pitch_node) {
    {
        vec3 vec(0, 0, -1);
        quat rot(vec3(radians(45.0f), 0, 0));
        vec3 out = rotate(rot, vec);
        vec3 expected = normalize(vec3(0.0f, 1.0f, -1.0f));
        EXPECT_VE3_EQ(out, expected);
    }
    {
        auto node = Node::create();
        node->rotate(quat(vec3(radians(45.0f), 0.0f, 0.0f)));
        vec3 forward = node->forwardVectorWorld();
        vec3 expected = normalize(vec3(0.0f, 1.0f, -1.0f));
        EXPECT_VE3_EQ(forward, expected);
    }
}

TEST(node_transform, local_transform_ref) {
    auto root = Node::create();
    root->createChildren({ "A", "B", "C", "D" });
    auto  transform = root->findFirstNodeByName("C")->localTransformRef();
    std::weak_ptr<Node> weakref = root->findFirstNodeByName("C");

    root.reset();

    EXPECT_FALSE(weakref.expired());
}

TEST(node_transform, rotate_axis) {
    float e = 0.000001f;
    auto n = Node::create("n");
    n->rotateY(glm::quarter_pi<float>());
    n->rotateY(glm::quarter_pi<float>());
    vec4 result = vec4(0, 0, 1, 1) * n->worldMatrix();
    EXPECT_FLOAT_CLOSE(result.x, -1.0f, FLOAT_ERR);
    EXPECT_FLOAT_CLOSE(result.y, 0.0f, FLOAT_ERR);
    EXPECT_FLOAT_CLOSE(result.z, 0.0f, FLOAT_ERR);
}

TEST(node_transform, set_translation) {
    auto n = Node::create("n");
    auto v1 = vec3(10, 20, 30);
    auto v2 = vec3(534, -54.054, 865.43f);
    auto v3 = vec3(93.27149192724909f, -54.054, 865.43f);
    n->setTranslation(v1);
    EXPECT_VE3_EQ(n->localTransform().translation(), v1);
    n->setTranslation(v2);
    EXPECT_VE3_EQ(n->localTransform().translation(), v2);
    n->setTranslation(v3.x, v3.y, v3.z);
    EXPECT_VE3_EQ(n->localTransform().translation(), v3);
}

TEST(node_transform, set_scale) {
    auto n = Node::create("n");
    auto v1 = vec3(10, 20, 30);
    auto v2 = vec3(534, -54.054, 865.43f);
    auto v3 = vec3(93.27149192724909f, -54.054, 865.43f);
    n->setScale(v1);
    EXPECT_VE3_EQ(n->localTransform().scale(), v1);
    n->setScale(v2);
    EXPECT_VE3_EQ(n->localTransform().scale(), v2);
    n->setScale(v3.x, v3.y, v3.z);
    EXPECT_VE3_EQ(n->localTransform().scale(), v3);
}

TEST(node_transform, set_rotation) {
    auto n = Node::create();
    n->rotateX(PI / 8.0f);
    n->rotateY(PI / 2.0f);
    n->rotateZ(PI / 16.0f);
    quat q(-0.353553414f, 0.353553414f, 0.146446630f, 0.853553355f);
    n->setRotation(q);
    auto r = n->localTransform().rotation();
    EXPECT_FLOAT_EQ(r.x, q.x);
    EXPECT_FLOAT_EQ(r.y, q.y);
    EXPECT_FLOAT_EQ(r.z, q.z);
    EXPECT_FLOAT_EQ(r.w, q.w);

    //n->getLocalTransform().getRotation();
    //void setRotation(const glm::quat& rotation);
    //void setRotationFromEuler(const vec3& eulerAngles);
    //void setRotationFromEuler(float pitch, float yaw, float roll);
}

TEST(node_transform, get_world) {
    auto root = Node::create("root");
    auto n1 = root->createChild("1");
    auto n2 = n1->createChild("2");
    auto n3 = n2->createChild("3");
    auto n4 = n3->createChild("4");
    auto n5 = n4->createChild("5");
    auto n6 = n5->createChild("6");


    root->translate(1, 0, 0);
    n1->translate(0, 1, 0);
    n2->translate(0, 0, 1);
    auto n3Transform = n3->worldTransform();
    auto n4Transform = n4->worldTransform();

}

TEST(node, world_rotation) {
    auto root = Node::create("root");
    auto middle = root->createChild();
    auto n1 = middle->createChild("n1");
    //n1->translate(1, 0, 0);
    root->rotateY(PI / 2.0f);
    root->rotateX(PI / 2.0f);

    auto t = n1->worldTransform();
    auto m = n1->worldMatrix();

    auto v = m * vec4(1, 0, 0, 1);
    EXPECT_TRUE(glm::abs(v.x) < FLOAT_ERR);
    EXPECT_TRUE(glm::abs(1.0f - v.y) < FLOAT_ERR);
    EXPECT_TRUE(glm::abs(v.z) < FLOAT_ERR);
}

TEST(node, set_matrix) {
    // Tests setting a node's transform from a matrix.
    // This was a bug with using glm::decompose incorrectly.
    auto n = Node::create();
    auto vec = vec4(1, 0, 1, 1);
    mat4 matrix(1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1);
    n->setLocalTransform(matrix);
    n->rotate(glm::quat()); // force rotation to be dirty
    auto matrixResult = matrix * vec;
    auto nodeResult = n->worldMatrix() * vec;

    EXPECT_FLOAT_CLOSE(matrixResult.x, nodeResult.x, FLOAT_ERR);
    EXPECT_FLOAT_CLOSE(matrixResult.y, nodeResult.y, FLOAT_ERR);
    EXPECT_FLOAT_CLOSE(matrixResult.z, nodeResult.z, FLOAT_ERR);
}
