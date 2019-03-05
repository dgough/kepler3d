#pragma once

#include "gtest/gtest.h"

#include <OpenGL.hpp>
#include <GLFW/glfw3.h>

namespace kepler {

static constexpr int WINDOW_WIDTH = 8;
static constexpr int WINDOW_HEIGHT = 6;

class KeplerEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        _window = createWindow();
    }
    void TearDown() override {
        if (_window != nullptr) {
            glfwDestroyWindow(_window);
        }
    }

    static GLFWwindow* createWindow() {
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

private:
    GLFWwindow* _window = nullptr;
};

} // namespace kepler
