#include "gtest/gtest.h"

#include <OpenGL.hpp>

#include <GLFW/glfw3.h>

#include <Node.hpp>
#include <Scene.hpp>
#include <Camera.hpp>
#include <BmpFont.hpp>
#include <App.hpp>

using namespace kepler;
using namespace kepler::gl;

//constexpr char* ARIAL_32 = "res/fonts/arial-32.fnt";
constexpr char* ARIAL_15 = "res/fonts/arial-15.fnt";

//#define DISABLE_FONT_TEST
#ifndef DISABLE_FONT_TEST

TEST(fonts, file_not_found) {
    auto font = BmpFont::createFromFile("asdf.fnt");
    EXPECT_EQ(font, nullptr);
}

TEST(fonts, load_font) {
    auto font = BmpFont::createFromFile(ARIAL_15);
    ASSERT_NE(font, nullptr);
    EXPECT_EQ(font->size(), 15);
}

#endif
