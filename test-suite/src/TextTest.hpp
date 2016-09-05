#pragma once

#include <AppDelegate.hpp>

/// Test loading fonts and drawing text.
/// Dropping a font file into the window will load it.
class TextTest : public kepler::AppDelegate {
public:
    TextTest();
    ~TextTest();

    void start() override;
    void update() override;
    void render() override;
    void keyEvent(int key, int scancode, int action, int mods) override;
    void mouseEvent(double xpos, double ypos) override;
    void mouseButtonEvent(int button, int action, int mods) override;
    void scrollEvent(double xoffset, double yoffset) override;
    void dropEvent(int count, const char** paths) override;

private:

    kepler::BmpFontRef _font;
};
