#pragma once

#include "gtest/gtest.h"

#include <OpenGL.hpp>
#include <GLFW/glfw3.h>
#include <App.hpp>

namespace kepler {

constexpr int WINDOW_WIDTH = 8;
constexpr int WINDOW_HEIGHT = 6;

class KeplerEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        _app = std::make_unique<App>(WINDOW_WIDTH, WINDOW_HEIGHT, false);
    }
    void TearDown() override {
        _app.reset();
    }

private:
    std::unique_ptr<App> _app;
};

} // namespace kepler
