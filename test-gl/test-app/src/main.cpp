#include <BaseGL.hpp>

#include <App.hpp>

#include "TestApp.hpp"

using namespace kepler;
using namespace kepler::gl;

static constexpr int WINDOW_WIDTH = 1024;
static constexpr int WINDOW_HEIGHT = 768;
static constexpr bool FULLSCREEN = false;

int main() {
    App app(WINDOW_WIDTH, WINDOW_HEIGHT, FULLSCREEN);
    app.setDelegate(std::make_shared<TestApp>());
    app.mainLoop();
    return 0;
}
