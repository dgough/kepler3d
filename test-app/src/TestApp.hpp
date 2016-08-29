#pragma once

#include <Base.hpp>
#include <AppDelegate.hpp>
#include <Transform.hpp>
#include <BmpFont.hpp>

using namespace kepler;

class TestApp : public AppDelegate
{
public:
    TestApp();
    ~TestApp();

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
    SceneRef loadDuckScene();
    void loadCityScene();

private:
    SceneRef _scene;
    std::unique_ptr<FirstPersonController> _firstPerson;
    std::unique_ptr<AxisCompass> _compass;
    BmpFontRef _font;
};
