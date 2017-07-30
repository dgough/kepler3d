#pragma once

#include <AppDelegate.hpp>
#include <OrbitCamera.hpp>
#include <AxisCompass.hpp>
#include <BoundingBox.hpp>

using namespace kepler;

/// Test loading a glTF 2.0 
/// Dropping a gltf file into the window will load it.
class Gltf2Test : public kepler::AppDelegate {
public:
    Gltf2Test();

    void start() override;
    void update() override;
    void render() override;
    void keyEvent(int key, int scancode, int action, int mods) override;
    void mouseEvent(double xpos, double ypos) override;
    void mouseButtonEvent(int button, int action, int mods) override;
    void scrollEvent(double xoffset, double yoffset) override;
    void dropEvent(int count, const char** paths) override;

private:
    void focus();
    void loadSceneFromFile(const char* path);
    void loadNextPath();
    void loadPrevPath();
    void calcBoundingBox();

private:
    bool _moveCamera;
    ref<Scene> _scene;
    ref<BmpFont> _font;
    AxisCompass _compass;
    OrbitCamera _orbitCamera;
    BoundingBox _box;
};
