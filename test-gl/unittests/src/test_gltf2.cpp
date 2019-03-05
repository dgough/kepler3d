#include "common_test.hpp"

#include <OpenGL.hpp>

#include <GLFW/glfw3.h>

#include <Node.hpp>
#include <Scene.hpp>
#include <Camera.hpp>
#include <GLTF2Loader.hpp>
#include <Material.hpp>

#define BASE_DIR "../../../glTF-Sample-Models/2.0/"

static constexpr char* BOX_PATH = BASE_DIR "Box/glTF/Box.gltf";
static constexpr char* LANTERN_PATH = BASE_DIR "Lantern/glTF/Lantern.gltf";
static constexpr char* CAMERA_PATH = BASE_DIR "Cameras/glTF/Cameras.gltf";
static constexpr char* ANIMATED_BOX_PATH = BASE_DIR "BoxAnimated/glTF/BoxAnimated.gltf";
static constexpr char* DUCK_PATH = BASE_DIR "Duck/glTF/Duck.gltf";
static constexpr char* TRUCK_PATH = BASE_DIR "CesiumMilkTruck/glTF/CesiumMilkTruck.gltf";

using namespace kepler;
using namespace kepler::gl;

//#define DISABLE_GLTF2_LOADER_TEST
#ifndef DISABLE_GLTF2_LOADER_TEST

//TEST(gltf, file_not_found) {
//    GLTF2Loader gltf;
//    auto scene = gltf.loadSceneFromFile("asdf.gltf");
//    EXPECT_EQ(scene, nullptr);
//}

TEST(gltf2, load_gltf2) {
    // load box
    {
        //GLTF2Loader gltf;
        //auto scene = gltf.loadSceneFromFile(BOX_PATH);
        //ASSERT_TRUE(scene != nullptr);
    }

    // load lantern
    {
        GLTF2Loader gltf;
        auto scene = gltf.loadSceneFromFile(LANTERN_PATH);
        ASSERT_TRUE(scene != nullptr);
        EXPECT_EQ(scene->childCount(), 1);
        auto node = scene->childAt(0);
        EXPECT_TRUE(node);
        EXPECT_EQ(node->childCount(), 3);
        EXPECT_STREQ(node->namePtr(), "Lantern");
    }
    // camera test
    {
        GLTF2Loader gltf;
        auto scene = gltf.loadSceneFromFile(CAMERA_PATH);
        ASSERT_TRUE(scene != nullptr);
        EXPECT_EQ(scene->childCount(), 3);
        // perspective camera
        auto pers = scene->childAt(1)->component<Camera>();
        EXPECT_TRUE(pers != nullptr);
        EXPECT_EQ(pers->cameraType(), Camera::Type::PERSPECTIVE);
        EXPECT_FLOAT_EQ(pers->aspectRatio(), 1.0f);
        EXPECT_FLOAT_EQ(pers->fov(), glm::degrees(0.7f));
        EXPECT_FLOAT_EQ(pers->far(), 100.0f);
        EXPECT_FLOAT_EQ(pers->near(), 0.01f);
        // ortho camera
        auto ortho = scene->childAt(2)->component<Camera>();
        EXPECT_TRUE(ortho);
        EXPECT_EQ(ortho->cameraType(), Camera::Type::ORTHOGRAPHIC);
        EXPECT_FLOAT_EQ(ortho->zoomX(), 1.0f);
        EXPECT_FLOAT_EQ(ortho->zoomY(), 1.0f);
        EXPECT_FLOAT_EQ(ortho->far(), 100.0f);
        EXPECT_FLOAT_EQ(ortho->near(), 0.01f);

        // node rotation
        auto rot = scene->childAt(0)->localTransform().rotation();
        auto expectedRot = glm::quat(0.92375f, -0.383f, 0.0f, 0.0f); // w is first
        EXPECT_QUAT_EQ(rot, expectedRot);
        // node translation
        EXPECT_VE3_EQ(scene->childAt(1)->localTransform().translation(), vec3(0.5f, 0.5f, 3.0f));
    }

    //// load_box
    //{
    //    GLTF2Loader gltf;
    //    auto scene = gltf.loadSceneFromFile(BOX_PATH);
    //    ASSERT_TRUE(scene != nullptr);
    //    EXPECT_EQ(scene->childCount(), 1);

    //    auto root = scene->childAt(0);
    //    EXPECT_EQ(root->name(), "Y_UP_Transform");
    //    EXPECT_EQ(root->childCount(), 1);

    //    auto child = root->childAt(0);
    //    EXPECT_EQ(child->name(), "Mesh");
    //    EXPECT_EQ(child->childCount(), 0);

    //    auto mesh = gltf.findMeshById("Geometry-mesh002");
    //    ASSERT_TRUE(mesh != nullptr);
    //}

    //// animated_box
    //{
    //    GLTF2Loader gltf;
    //    auto scene = gltf.loadSceneFromFile(ANIMATED_BOX_PATH);
    //    ASSERT_TRUE(scene != nullptr);
    //    EXPECT_EQ(scene->childCount(), 5);
    //}

    //// load duck
    //{
    //    GLTF2Loader gltf;
    //    auto scene = gltf.loadSceneFromFile(DUCK_PATH);
    //    ASSERT_TRUE(scene != nullptr);

    //    auto cameraNode = scene->findFirstNodeByName("camera1");
    //    ASSERT_TRUE(cameraNode != nullptr);
    //    auto camera = cameraNode->component<Camera>();
    //    ASSERT_TRUE(camera != nullptr);
    //    EXPECT_EQ(camera->cameraType(), Camera::Type::PERSPECTIVE);
    //}

    //// load truck
    //{
    //    GLTF2Loader gltf;
    //    auto loadResult = gltf.load(TRUCK_PATH);
    //    EXPECT_TRUE(loadResult);

    //    // test getMaterialByName
    //    const std::string wheelsName("wheels");
    //    auto wheelsMaterial = gltf.findMaterialByName(wheelsName);
    //    ASSERT_TRUE(wheelsMaterial != nullptr);
    //    EXPECT_EQ(wheelsName, wheelsMaterial->name());
    //}

    //glfwTerminate();
}

#endif
