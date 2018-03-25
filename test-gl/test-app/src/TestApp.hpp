#pragma once

#include <BaseGL.hpp>
#include <AppDelegate.hpp>
#include <Transform.hpp>
#include <BmpFont.hpp>

namespace kepler {
namespace gl {

class TestApp : public AppDelegate {
public:
    TestApp();

    void start() override;
    void update() override;
    void render() override;
    void keyEvent(int key, int scancode, int action, int mods) override;
    void mouseEvent(double xpos, double ypos) override;
    void mouseButtonEvent(int button, int action, int mods) override;
    void scrollEvent(double xoffset, double yoffset) override;
    void dropEvent(int count, const char** paths) override;

private:
    void doMovement();
    void drawText();

    void loadGLTF(const char* path);
    void loadScenes();
    ref<Scene> loadDuckScene();

private:
    ref<Scene> _scene;
    std::unique_ptr<FirstPersonController> _firstPerson;
    std::unique_ptr<AxisCompass> _compass;
    ref<BmpFont> _font;
};
}
}
