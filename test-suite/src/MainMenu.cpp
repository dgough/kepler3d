#include "MainMenu.hpp"
#include "SceneTest.hpp"
#include "TextTest.hpp"
#include "LightTest.hpp"

#include <BaseGL.hpp>
#include <GLFW/glfw3.h>

#include <App.hpp>
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
    app()->setSwapInterval(1);

    if (_buttons.empty()) {
        static constexpr float width = 300.f;
        float height = static_cast<float>(_font->getSize()) * _font->getScale() + 6.f;
        Rectangle rect(100.f, 100.f, width, height);

        addButton("Scene Test", rect, []() {
            app()->setDelegate(std::make_shared<SceneTest>());
        });

        rect.pos.y += height;
        addButton("Text Test", rect, []() {
            app()->setDelegate(std::make_shared<TextTest>());
        });

        rect.pos.y += height;
        addButton("Light Test", rect, []() {
            app()->setDelegate(std::make_shared<LightTest>());
        });
    }
}

void MainMenu::update() {
}

void MainMenu::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float height = app()->getHeightFloat();

    float fontSize = static_cast<float>(_font->getSize());

    for (auto& button : _buttons) {
        drawButton(*button);
    }
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
        double xpos, ypos;
        app()->getCursorPos(&xpos, &ypos);
        clickButtonAt(glm::vec2(xpos, ypos));
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

void MainMenu::addButton(const char* text, const Rectangle& rect, std::function<void()> callback) {
    auto button = std::make_shared<Button>(text, rect);
    button->setOnClick(callback);
    _buttons.push_back(button);
}

void MainMenu::drawButton(const Button& button) {
    float x = button.getRect().x();
    float y = button.getRect().y();
    _font->drawText(button.getText().c_str(), x, y);
}

bool MainMenu::clickButtonAt(glm::vec2 pos) {
    for (auto button : _buttons) {
        if (button->getRect().contains(pos)) {
            button->callOnClick();
            return true;
        }
    }
    return false;
}

int main() {
    App app(WIDTH, HEIGHT, FULLSCREEN);
    MainMenu::gotoMainMenu();
    app.mainLoop();
    return 0;
}
