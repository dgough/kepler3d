#include "MainMenu.hpp"
#include "SceneTest.hpp"

#include <BaseGL.hpp>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <App.hpp>
#include <Node.hpp>
#include <Transform.hpp>
#include <Camera.hpp>
#include <Effect.hpp>
#include <VertexBuffer.hpp>
#include <IndexBuffer.hpp>
#include <Scene.hpp>
#include <Mesh.hpp>
#include <MeshPrimitive.hpp>
#include <Material.hpp>
#include <Technique.hpp>
#include <VertexAttributeAccessor.hpp>
#include <MeshRenderer.hpp>
#include <FirstPersonController.hpp>
#include <Image.hpp>
#include <Texture.hpp>
#include <Sampler.hpp>
#include <BmpFont.hpp>

#include <iostream>

using namespace kepler;

static constexpr int WIDTH = 1024;
static constexpr int HEIGHT = 768;
static constexpr bool FULLSCREEN = false;

static std::shared_ptr<MainMenu> __instance;

MainMenu::MainMenu() {
    _font = BmpFont::createFromFile("res/fonts/arial-32.fnt");
    if (!_font) {
        throw std::runtime_error("Failed to load font.");
    }
}

MainMenu::~MainMenu() {
}

void MainMenu::start() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void MainMenu::update() {
}

void MainMenu::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float height = app()->getHeightFloat();

    float fontSize = static_cast<float>(_font->getSize());

    _font->drawText("Scene Test", 0, 0);
}

void MainMenu::keyEvent(int key, int scancode, int action, int mods) {
    if (key == KEY_ESCAPE && action == PRESS) {
        app()->setShouldClose(true);
    }
}

void MainMenu::mouseEvent(double xpos, double ypos) {
}

void MainMenu::mouseButtonEvent(int button, int action, int mods) {
    if (button == LEFT_MOUSE && action == PRESS) {
        app()->setDelegate(std::make_shared<SceneTest>());
    }
}

std::shared_ptr<MainMenu> MainMenu::getInstance() {
    if (!__instance) {
        __instance = std::make_shared<MainMenu>();
    }
    return __instance;
}

void MainMenu::gotoMainMenu() {
    app()->setDelegate(getInstance());
}

int main() {
    App app(WIDTH, HEIGHT, FULLSCREEN);
    MainMenu::gotoMainMenu();
    app.mainLoop();
    return 0;
}
