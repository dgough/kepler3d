#pragma once

#include <AppDelegate.hpp>
#include <OrbitCamera.hpp>
#include <AxisCompass.hpp>

using namespace kepler;

class LightTest : public kepler::AppDelegate {
public:
    LightTest();
    ~LightTest();

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
    bool _moveCamera;
    ref<Scene> _scene;
    ref<Node> _lightParent;
    ref<BmpFont> _font;
    AxisCompass _compass;
    OrbitCamera _orbitCamera;
};
