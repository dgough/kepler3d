#pragma once

#include <AppDelegate.hpp>
#include <Button.hpp>

#include <vector>

using namespace kepler;

/// The main menu of the test suite.
class MainMenu : public AppDelegate {
public:
    /// Use getInstance() instead.
    MainMenu();
    ~MainMenu();

    void start() override;
    void update() override;
    void render() override;
    void keyEvent(int key, int scancode, int action, int mods) override;
    void mouseEvent(double xpos, double ypos) override;
    void mouseButtonEvent(int button, int action, int mods) override;

    static std::shared_ptr<MainMenu> getInstance();
    static void gotoMainMenu();

private:
    void addButton(const char* text, const Rectangle& rect, std::function<void()> callback);
    void drawButton(const Button& button);
    bool clickButtonAt(glm::vec2 pos);

    BmpFontRef _font;
    std::vector<ButtonRef> _buttons;
};
