#include "MainMenu.hpp"
#include "TextTest.hpp"
#include "LightTest.hpp"
#include "Gltf2Test.hpp"

#include <OpenGL.hpp>
#include <GLFW/glfw3.h>

#include <App.hpp>
#include <BmpFont.hpp>

#include <iostream>

namespace kepler {
namespace gl {

static constexpr int WINDOW_WIDTH = 1024;
static constexpr int WINDOW_HEIGHT = 768;
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
    app()->setSwapInterval(1);

    if (_buttons.empty()) {
        static constexpr float width = 300.f;
        float height = static_cast<float>(_font->size()) * _font->scale() + 6.f;
        Rectangle rect(100.f, 100.f, width, height);

        rect.pos.y += height;
        addButton("Text Test", rect, []() {
            app()->setDelegate(std::make_shared<TextTest>());
        });

        rect.pos.y += height;
        addButton("Light Test", rect, []() {
            app()->setDelegate(std::make_shared<LightTest>());
        });

        rect.pos.y += height;
        addButton("glTF 2.0 Scene Test", rect, []() {
            app()->setDelegate(std::make_shared<Gltf2Test>());
        });
    }
}

void MainMenu::update() {
}

void MainMenu::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (const auto& button : _buttons) {
        drawButton(*button);
    }
}

void MainMenu::keyEvent(int key, int scancode, int action, int mods) {
    if (key == KEY_ESCAPE && action == PRESS) {
        app()->setShouldClose(true);
        __instance.reset();
    }
}

void MainMenu::mouseEvent(double xpos, double ypos) {
}

void MainMenu::mouseButtonEvent(int button, int action, int mods) {
    if (button == LEFT_MOUSE && action == PRESS) {
        double xpos, ypos;
        app()->cursorPosition(&xpos, &ypos);
        clickButtonAt(vec2(xpos, ypos));
    }
}

std::shared_ptr<MainMenu> MainMenu::instance() {
    if (!__instance) {
        __instance = std::make_shared<MainMenu>();
    }
    return __instance;
}

void MainMenu::gotoMainMenu() {
    app()->setDelegate(instance());
}

void MainMenu::addButton(const char* text, const Rectangle& rect, std::function<void()> callback) {
    auto button = std::make_shared<Button>(text, rect);
    button->setOnClick(callback);
    _buttons.push_back(button);
}

void MainMenu::drawButton(const Button& button) {
    float x = button.rect().x();
    float y = button.rect().y();
    _font->drawText(button.text().c_str(), x, y);
}

bool MainMenu::clickButtonAt(vec2 pos) {
    for (const auto& button : _buttons) {
        if (button->rect().contains(pos)) {
            button->callOnClick();
            return true;
        }
    }
    return false;
}
}
}

int main() {
    using namespace kepler;
    using namespace kepler::gl;
    App app(WINDOW_WIDTH, WINDOW_HEIGHT, FULLSCREEN);
    MainMenu::gotoMainMenu();
    app.mainLoop();
    return 0;
}