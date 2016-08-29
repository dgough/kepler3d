#pragma once

#include <AppDelegate.hpp>
#include <OrbitCamera.hpp>
#include <AxisCompass.hpp>

/// Test loading a single scene.
/// Dropping a gltf file into the window will load it.
class SceneTest : public kepler::AppDelegate {
public:
    SceneTest();
    ~SceneTest();

    void start() override;
    void update() override;
    void render() override;
    void keyEvent(int key, int scancode, int action, int mods) override;
    void mouseEvent(double xpos, double ypos) override;
    void mouseButtonEvent(int button, int action, int mods) override;
    void scrollEvent(double xoffset, double yoffset) override;
    void dropEvent(int count, const char** paths) override;

private:
    void loadSceneFromFile(const char* path);

private:
    bool _moveCamera;
    kepler::SceneRef _scene;
    kepler::NodeRef _cameraNode;
    kepler::BmpFontRef _font;
    kepler::AxisCompass _compass;
    kepler::OrbitCamera _orbitCamera;
};
