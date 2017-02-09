#pragma once

#include <AppDelegate.hpp>
#include <Button.hpp>

#include <vector>

using namespace kepler;

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
    bool clickButtonAt(glm::vec2 pos);

    ref<BmpFont> _font;
    std::vector<ref<Button>> _buttons;
};
