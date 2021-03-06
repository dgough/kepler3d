#include "stdafx.h"
#include <GLFW/glfw3.h>
#include <AppVk.hpp>
#include <VulkanState.hpp>

#include <string>

namespace kepler {

namespace vulkan {
static VulkanState* g_vk;
VulkanState& vulkanState() {
    return *g_vk;
}
}

using vulkan::VulkanState;
using vulkan::g_vk;

static App* g_app = nullptr;

App* app() {
    return g_app;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void drop_callback(GLFWwindow* window, int count, const char** path);

static double g_deltaTime;
static double g_currentTime;
static double g_prevTime;

double deltaTime() {
    return g_deltaTime;
}



// Private implementation
/// @Internal
class App::Impl {
    friend App;
public:
    Impl(int width = 0, int height = 0, bool fullscreen = false);
    ~Impl() noexcept;

private:
    static void onWindowResized(GLFWwindow* window, int width, int height);

    GLFWwindow* createWindow(int width, int height);
    void mainLoop();

private:
    int _width;
    int _height;
    bool _fullscreen;

    std::string _title;

    shared_ptr<AppDelegate> _delegate;
    size_t _frameCount;

    GLFWwindow* _window;
    VulkanState _vk;
};

////

App::App() : _impl(std::make_unique<Impl>()) {
    g_app = this;
}

App::App(int width, int height, bool fullscreen) : _impl(std::make_unique<Impl>(width, height, fullscreen)) {
    g_app = this;
}

App::~App() noexcept {
    setDelegate(nullptr);
    g_app = nullptr;
}

void App::setDelegate(const shared_ptr<AppDelegate>& appDelegate) {
    auto prev = _impl->_delegate;
    _impl->_delegate = appDelegate;
    if (prev) {
        prev->stop();
    }
    if (appDelegate) {
        appDelegate->start();
    }
}

void App::mainLoop() {
    _impl->mainLoop();
}

void App::keyEvent(int key, int scancode, int action, int mods) {
    if (_impl->_delegate) {
        _impl->_delegate->keyEvent(key, scancode, action, mods);
    }
}

void App::mouseEvent(double xpos, double ypos) {
    if (_impl->_delegate) {
        _impl->_delegate->mouseEvent(xpos, ypos);
    }
}

void App::mouseButtonEvent(int button, int action, int mods) {
    if (_impl->_delegate) {
        _impl->_delegate->mouseButtonEvent(button, action, mods);
    }
}

void App::scrollEvent(double xoffset, double yoffset) {
    if (_impl->_delegate) {
        _impl->_delegate->scrollEvent(xoffset, yoffset);
    }
}

void App::dropEvent(int count, const char** paths) {
    if (_impl->_delegate) {
        _impl->_delegate->dropEvent(count, paths);
    }
}

int App::getKey(int key) {
    return glfwGetKey(_impl->_window, key);
}

int App::getMouseButton(int button) {
    return glfwGetMouseButton(_impl->_window, button);
}

void App::cursorPosition(double* xpos, double* ypos) {
    glfwGetCursorPos(_impl->_window, xpos, ypos);
}

int App::width() const {
    return _impl->_width;
}

float App::widthAsFloat() const {
    return static_cast<float>(_impl->_width);
}

int App::height() const {
    return _impl->_height;
}

float App::heightAsFloat() const {
    return static_cast<float>(_impl->_height);
}

void App::windowSize(int& width, int& height) {
    glfwGetWindowSize(_impl->_window, &width, &height);
}

float App::aspectRatio() const {
    if (_impl->_height == 0.0f) {
        return 0.0f;
    }
    return static_cast<float>(_impl->_width) / static_cast<float>(_impl->_height);
}

size_t App::frameCount() const noexcept {
    return _impl->_frameCount;
}

void App::setShouldClose(bool value) {
    glfwSetWindowShouldClose(_impl->_window, value);
}

void App::setSwapInterval(int interval) {
    glfwSwapInterval(interval);
}

void App::setCursorVisibility(bool visible) {
    glfwSetInputMode(_impl->_window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

bool App::isCursorVisible() const {
    return glfwGetInputMode(_impl->_window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED;
}

App* App::instance() {
    return g_app;
}

////

App::Impl::Impl(int width, int height, bool fullscreen)
    : _width(width), _height(height), _fullscreen(fullscreen), _frameCount(0),
    _window(createWindow(width, height)), _vk(_window) {
    g_vk = &_vk; // TODO fix this
}

App::Impl::~Impl() noexcept {
    _vk.cleanup();
    // glfw
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void App::Impl::onWindowResized(GLFWwindow* window, int width, int height) {
    if (width == 0 || height == 0) return;

    App::Impl* app = reinterpret_cast<App::Impl*>(glfwGetWindowUserPointer(window));
    app->_vk.recreateSwapChain();
}

GLFWwindow* App::Impl::createWindow(int width, int height) {
    if (width == 0 || height == 0) {
        //_fullscreen = true;
        _width = 800;
        _height = 600;
    }
    if (_title.empty()) {
        _title.assign("App");
    }

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWmonitor* monitor = nullptr;
    if (_fullscreen) {
        monitor = glfwGetPrimaryMonitor();
        auto mode = glfwGetVideoMode(monitor);
        _width = mode->width;
        _height = mode->height;
    }
    _window = glfwCreateWindow(_width, _height, _title.c_str(), monitor, nullptr);

    // Set callbacks
    glfwSetKeyCallback(_window, key_callback);
    glfwSetCursorPosCallback(_window, mouse_callback);
    glfwSetMouseButtonCallback(_window, mouse_button_callback);
    glfwSetScrollCallback(_window, scroll_callback);
    glfwSetDropCallback(_window, drop_callback);
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glfwSetWindowUserPointer(_window, this);
    glfwSetWindowSizeCallback(_window, onWindowResized);

    return _window;
}

void App::Impl::mainLoop() {
    g_prevTime = glfwGetTime();
    while (!glfwWindowShouldClose(_window)) {
        ++_frameCount;
        glfwPollEvents();
        g_currentTime = glfwGetTime();
        g_deltaTime = g_currentTime - g_prevTime;
        g_prevTime = g_currentTime;

        // If depth writes were disabled then glClear won't clear the depth buffer.
        // This can happen if the last object to be drawn disabled depth writes.
        //RenderState::setGlobalDepthMask(true);
        if (_delegate) {
            _delegate->update();
            _delegate->render();
        }
    }
    _vk.deviceWaitIdle();
}

void key_callback(GLFWwindow*, int key, int scancode, int action, int mods) {
    g_app->keyEvent(key, scancode, action, mods);
}

void mouse_callback(GLFWwindow*, double xpos, double ypos) {
    g_app->mouseEvent(xpos, ypos);
}

void mouse_button_callback(GLFWwindow*, int button, int action, int mods) {
    g_app->mouseButtonEvent(button, action, mods);
}

void scroll_callback(GLFWwindow*, double xoffset, double yoffset) {
    g_app->scrollEvent(xoffset, yoffset);
}

void drop_callback(GLFWwindow*, int count, const char** paths) {
    g_app->dropEvent(count, paths);
}
}
