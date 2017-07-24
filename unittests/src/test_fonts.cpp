#include "gtest/gtest.h"

#include <BaseGL.hpp>

#include <GLFW/glfw3.h>

#include <Node.hpp>
#include <Scene.hpp>
#include <Camera.hpp>
#include <BmpFont.hpp>
#include <App.hpp>

using namespace kepler;

static constexpr char* ARIAL_32 = "res/fonts/arial-32.fnt";
static constexpr char* ARIAL_15 = "res/fonts/arial-15.fnt";

static constexpr int WINDOW_WIDTH = 8;
static constexpr int WINDOW_HEIGHT = 6;

#define DISABLE_FONT_TEST
#ifndef DISABLE_FONT_TEST

TEST(fonts, file_not_found) {
    auto font = BmpFont::createFromFile("asdf.fnt");
    EXPECT_EQ(font, nullptr);
}

TEST(fonts, load_font) {
    App app(WINDOW_WIDTH, WINDOW_HEIGHT, false);
    {
        auto font = BmpFont::createFromFile(ARIAL_15);
        ASSERT_NE(font, nullptr);
        EXPECT_EQ(font->size(), 15);
    }
}

#endif