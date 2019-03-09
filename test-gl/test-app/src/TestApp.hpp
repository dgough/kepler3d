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
    ~TestApp() noexcept override;

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
    shared_ptr<Scene> loadDuckScene();

private:
    shared_ptr<Scene> _scene;
    std::unique_ptr<FirstPersonController> _firstPerson;
    std::unique_ptr<AxisCompass> _compass;
    shared_ptr<BmpFont> _font;
    shared_ptr<Node> _truck;
};

} // namespace gl
} // namespace kepler
