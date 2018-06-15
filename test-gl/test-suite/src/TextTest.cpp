#include "TextTest.hpp"
#include "MainMenu.hpp"

#include <OpenGL.hpp>
#include <App.hpp>
#include <StringUtils.hpp>
#include <BmpFont.hpp>

#include <iostream>
#include <chrono>

namespace kepler {
namespace gl {

using glm::vec3;

static constexpr char* FONT_EXTENSION = ".fnt";
static const char* FONT_PATH = "res/fonts/arial-32.fnt";

static std::string g_text;

static shared_ptr<BmpFont> loadFont(const char* path) {
    auto start = std::chrono::high_resolution_clock::now();

    auto font = BmpFont::createFromFile(path);
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::clog << static_cast<double>(time.count()) / 1000000.0;
    std::clog << " ms to load font file " << path << std::endl;
    g_text.assign(path);
    return font;
}

void TextTest::start() {
    //glClearColor(0.16f, 0.3f, 0.5f, 1.0f);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    app()->setSwapInterval(0);
    _font = loadFont(FONT_PATH);
}

void TextTest::update() {
}

void TextTest::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_font) {
        float height = _font->sizeAsFloat();
        static constexpr float x = 0.f;
        float y = 0.f;
        _font->drawText(g_text.c_str(), x, y);
        _font->drawText("Blue", x, y += height, glm::vec3(0, 0, 1));
        _font->drawText("gggHggg", x, y += height, glm::vec3(.5f, .62f, 1));
        _font->drawText("HHHHHH", x, y += height, glm::vec3(1, 0, 0));


        auto bottom = app()->heightAsFloat() - static_cast<float>(_font->lineHeight());
        _font->drawText("Drag font into window to load.", x, bottom, glm::vec3(0, 1, 1));
    }
}

void TextTest::keyEvent(int key, int scancode, int action, int mods) {
    if (key == KEY_ESCAPE && action == PRESS) {
        MainMenu::gotoMainMenu();
    }
}

void TextTest::mouseEvent(double xpos, double ypos) {
}

void TextTest::mouseButtonEvent(int button, int action, int mods) {
    if (button == LEFT_MOUSE && action == PRESS) {
    }
}

void TextTest::scrollEvent(double xoffset, double yoffset) {
}

void TextTest::dropEvent(int count, const char** paths) {
    // Load the first font file found. Ignore directories.
    for (int i = 0; i < count; ++i) {
        const char* path = paths[i];
        if (endsWith(path, FONT_EXTENSION)) {
            std::clog << "Load: " << path << std::endl;
            auto font = loadFont(path);
            if (font) {
                _font = font;
            }
            return;
        }
    }
}
}
}
