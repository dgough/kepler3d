#pragma once

#include <AppDelegate.hpp>
#include <OrbitCamera.hpp>
#include <AxisCompass.hpp>

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
    kepler::SceneRef _scene;
    kepler::NodeRef _lightParent;
    kepler::BmpFontRef _font;
    kepler::AxisCompass _compass;
    kepler::OrbitCamera _orbitCamera;
};
