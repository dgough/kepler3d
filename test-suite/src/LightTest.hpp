#pragma once

#include <BaseGL.hpp>
#include <AppDelegate.hpp>
#include <OrbitCamera.hpp>
#include <AxisCompass.hpp>

namespace kepler {
namespace gl {

class LightTest : public kepler::AppDelegate {
public:
    LightTest();

    void start() override;
    void update() override;
    void render() override;
    void keyEvent(int key, int scancode, int action, int mods) override;
    void mouseEvent(double xpos, double ypos) override;
    void mouseButtonEvent(int button, int action, int mods) override;
    void scrollEvent(double xoffset, double yoffset) override;

private:
    void loadSceneFromFile(const char* path);

private:
    ref<Scene> _scene;
    ref<Node> _lightParent;
    ref<BmpFont> _font;
    AxisCompass _compass;
    OrbitCamera _orbitCamera;
    bool _moveCamera{false};
    bool _pause{false};
};

}
}
