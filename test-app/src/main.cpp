#include <BaseGL.hpp>

#include <App.hpp>

#include "TestApp.hpp"

using namespace kepler;

int main() {
    App app(1024, 768, false);
    app.setDelegate(std::make_shared<TestApp>());
    app.mainLoop();
    return 0;
}
