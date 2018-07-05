#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <array>
#include <set>
#include <memory>

#include <App.hpp>
#include <AppVk.hpp>
#include <VulkanState.hpp>

using namespace kepler;
using namespace kepler::vulkan;

static constexpr int WINDOW_WIDTH = 1024;
static constexpr int WINDOW_HEIGHT = 768;
static constexpr bool FULLSCREEN = false;

class HelloTriangleApplication : public AppDelegate {
public:

    HelloTriangleApplication() 
        : _vk(vulkanState()) {
    }

    ~HelloTriangleApplication() {
    }

    void start() override {
    }

    void update() override {
        _vk.updateUniformBuffer();
    }

    void render() override {
        _vk.drawFrame();
    }

    void keyEvent(int key, int scancode, int action, int mods) override {
        if (key == KEY_ESCAPE && action == PRESS) {
            app()->setShouldClose(true);
        }
    }

private:
    VulkanState& _vk;
};

int main() {
    App app(WINDOW_WIDTH, WINDOW_HEIGHT, FULLSCREEN);
    app.setDelegate(std::make_shared<HelloTriangleApplication>());

    try {
        app.mainLoop();
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
