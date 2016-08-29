#include "gtest/gtest.h"

#include <BaseGL.hpp>

// GLFW
#include <GLFW/glfw3.h>

#include <Node.hpp>
#include <Scene.hpp>
#include <Camera.hpp>
#include <GLTFLoader.hpp>
#include <Material.hpp>

using namespace kepler;

static constexpr char* BOX_PATH = "C:/dev/github/glTF/sampleModels/Box/glTF/Box.gltf";
static constexpr char* ANIMATED_BOX_PATH = "C:/dev/github/glTF/sampleModels/BoxAnimated/glTF/BoxAnimated.gltf";
static constexpr char* DUCK_PATH = "C:/dev/github/glTF/sampleModels/Duck/glTF/Duck.gltf";
static constexpr char* TRUCK_PATH = "C:/dev/github/glTF/sampleModels/CesiumMilkTruck/glTF/CesiumMilkTruck.gltf";

constexpr int WIDTH = 8;
constexpr int HEIGHT = 6;


static GLFWwindow* setup() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "kepler3d", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return nullptr;
    }
    glViewport(0, 0, WIDTH, HEIGHT);
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
        EXPECT_TRUE(scene != nullptr);
        EXPECT_EQ(scene->getChildCount(), 1);

        auto root = scene->getChildAt(0);
        EXPECT_EQ(root->getName(), "Y_UP_Transform");
        EXPECT_EQ(root->getChildCount(), 1);

        auto child = root->getChildAt(0);
        EXPECT_EQ(child->getName(), "Mesh");
        EXPECT_EQ(child->getChildCount(), 0);

        auto mesh = gltf.getMeshById("Geometry-mesh002");
        EXPECT_TRUE(mesh != nullptr);
    }

    // animated_box
    {
        GLTFLoader gltf;
        auto scene = gltf.loadSceneFromFile(ANIMATED_BOX_PATH);
        EXPECT_TRUE(scene != nullptr);
        EXPECT_EQ(scene->getChildCount(), 5);
    }

    // load duck
    {
        GLTFLoader gltf;
        auto scene = gltf.loadSceneFromFile(DUCK_PATH);
        EXPECT_TRUE(scene != nullptr);

        auto cameraNode = scene->findFirstNodeByName("camera1");
        EXPECT_TRUE(cameraNode != nullptr);
        auto camera = cameraNode->getComponent<Camera>();
        EXPECT_TRUE(camera != nullptr);
        EXPECT_EQ(camera->getCameraType(), Camera::Type::PERSPECTIVE);
    }

    // load truck
    {
        GLTFLoader gltf;
        auto loadResult = gltf.load(TRUCK_PATH);
        EXPECT_TRUE(loadResult);

        // test getMaterialByName
        const std::string wheelsName("wheels");
        auto wheelsMaterial = gltf.getMaterialByName(wheelsName);
        EXPECT_TRUE(wheelsMaterial != nullptr);
        EXPECT_EQ(wheelsName, wheelsMaterial->getName());
    }

    glfwTerminate();
}
