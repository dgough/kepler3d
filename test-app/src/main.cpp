#include <BaseGL.hpp>

#include <App.hpp>

#include "TestApp.hpp"

using namespace kepler;

static constexpr int WIDTH = 1024;
static constexpr int HEIGHT = 768;
static constexpr bool FULLSCREEN = false;

int main() {
    App app(WIDTH, HEIGHT, FULLSCREEN);
    app.setDelegate(std::make_shared<TestApp>());
    app.mainLoop();
    return 0;
}
