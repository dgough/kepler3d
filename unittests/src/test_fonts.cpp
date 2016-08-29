#include "gtest/gtest.h"

#include <BaseGL.hpp>

// GLFW
#include <GLFW/glfw3.h>

#include <Node.hpp>
#include <Scene.hpp>
#include <Camera.hpp>
#include <BmpFont.hpp>
#include <App.hpp>

using namespace kepler;

static constexpr char* ARIAL_32 = "res/fonts/arial-32.fnt";
static constexpr char* ARIAL_15 = "res/fonts/arial-15.fnt";

constexpr int WIDTH = 8;
constexpr int HEIGHT = 6;

TEST(fonts, file_not_found) {
    auto font = BmpFont::createFromFile("asdf.fnt");
    EXPECT_EQ(font, nullptr);
}

TEST(fonts, load_font) {
    App app(WIDTH, HEIGHT, false);
    {
        auto font = BmpFont::createFromFile(ARIAL_15);
        EXPECT_NE(font, nullptr);
        EXPECT_EQ(font->getSize(), 15);
    }
}
