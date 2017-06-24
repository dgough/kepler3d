#include "gtest/gtest.h"

#include <BaseGL.hpp>

#include <GLFW/glfw3.h>

#include <Node.hpp>
#include <Scene.hpp>
#include <Camera.hpp>
#include <GLTFLoader.hpp>
#include <Material.hpp>

#define BASE_DIR "C:/dev/github/glTF-Sample-Models/1.0/"

static constexpr char* BOX_PATH = BASE_DIR "Box/glTF/Box.gltf";
static constexpr char* ANIMATED_BOX_PATH = BASE_DIR "BoxAnimated/glTF/BoxAnimated.gltf";
static constexpr char* DUCK_PATH = BASE_DIR "Duck/glTF/Duck.gltf";
static constexpr char* TRUCK_PATH = BASE_DIR "CesiumMilkTruck/glTF/CesiumMilkTruck.gltf";

static constexpr int WINDOW_WIDTH = 8;
static constexpr int WINDOW_HEIGHT = 6;

using namespace kepler;

//#define DISABLE_GLTF_LOADER_TEST
#ifndef DISABLE_GLTF_LOADER_TEST

static GLFWwindow* setup() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "kepler3d", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return nullptr;
    }
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glGetError(); // clear error flag
    return window;
}

TEST(gltf, file_not_found) {
    GLTFLoader gltf;
    auto scene = gltf.loadSceneFromFile("asdf.gltf");
    EXPECT_EQ(scene, nullptr);
}

TEST(gltf, load_gltf) {
    setup();
    // load_box    
    {
        GLTFLoader gltf;
        auto scene = gltf.loadSceneFromFile(BOX_PATH);
        ASSERT_TRUE(scene != nullptr);
        EXPECT_EQ(scene->childCount(), 1);

        auto root = scene->childAt(0);
        EXPECT_EQ(root->name(), "Y_UP_Transform");
        EXPECT_EQ(root->childCount(), 1);

        auto child = root->childAt(0);
        EXPECT_EQ(child->name(), "Mesh");
        EXPECT_EQ(child->childCount(), 0);

        auto mesh = gltf.findMeshById("Geometry-mesh002");
        ASSERT_TRUE(mesh != nullptr);
    }

    // animated_box
    {
        GLTFLoader gltf;
        auto scene = gltf.loadSceneFromFile(ANIMATED_BOX_PATH);
        ASSERT_TRUE(scene != nullptr);
        EXPECT_EQ(scene->childCount(), 5);
    }

    // load duck
    {
        GLTFLoader gltf;
        auto scene = gltf.loadSceneFromFile(DUCK_PATH);
        ASSERT_TRUE(scene != nullptr);

        auto cameraNode = scene->findFirstNodeByName("camera1");
        ASSERT_TRUE(cameraNode != nullptr);
        auto camera = cameraNode->component<Camera>();
        ASSERT_TRUE(camera != nullptr);
        EXPECT_EQ(camera->cameraType(), Camera::Type::PERSPECTIVE);
    }

    // load truck
    {
        GLTFLoader gltf;
        auto loadResult = gltf.load(TRUCK_PATH);
        EXPECT_TRUE(loadResult);

        // test getMaterialByName
        const std::string wheelsName("wheels");
        auto wheelsMaterial = gltf.findMaterialByName(wheelsName);
        ASSERT_TRUE(wheelsMaterial != nullptr);
        EXPECT_EQ(wheelsName, wheelsMaterial->name());
    }

    glfwTerminate();
}

#endif
