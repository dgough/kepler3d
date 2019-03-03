#pragma once

#include <BaseGL.hpp>
#include <AppDelegate.hpp>
#include <Button.hpp>

#include <vector>

namespace kepler {
namespace gl {

/// The main menu of the test suite.
class MainMenu : public AppDelegate {
public:
    /// Use instance() instead.
    MainMenu();
    ~MainMenu();

    void start() override;
    void update() override;
    void render() override;
    void keyEvent(int key, int scancode, int action, int mods) override;
    void mouseEvent(double xpos, double ypos) override;
    void mouseButtonEvent(int button, int action, int mods) override;

    static std::shared_ptr<MainMenu> instance();
    static void gotoMainMenu();

private:
    void addButton(const char* text, const Rectangle& rect, std::function<void()> callback);
    void drawButton(const Button& button);
    bool clickButtonAt(vec2 pos);

    shared_ptr<BmpFont> _font;
    std::vector<shared_ptr<Button>> _buttons;
};
}
}