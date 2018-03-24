#pragma once

#include "Base.hpp"
#include "AppDelegate.hpp"
#include "Platform.hpp"

namespace kepler {

/// The application class.
/// Don't extend this class, instead extend AppDelegate and set it as the active delegate in order to receive callbacks.
class App {
public:
    App();
    App(int width, int height, bool fullscreen);
    virtual ~App() noexcept;

    /// Sets the delegate that will receive callbacks.
    void setDelegate(const ref<AppDelegate>& appDelegate);

    /// Starts the main loop.
    void mainLoop();

    /// Keyboard event.
    /// @param[in] key      Keyboard key (See the Key enum).
    /// @param[in] scancode Scancode of the key.
    /// @param[in] action   RELEASE, PRESS or REPEAT.
    /// @param[in] mods     Modifier keys. (MOD_SHIFT, MOD_CTRL, MOD_ALT, MOD_SUPER)
    void keyEvent(int key, int scancode, int action, int mods);

    /// Mouse movement event. Top left is (0, 0).
    /// @param[in] xpos X position in the client area.
    /// @param[in] ypos Y position in the client area.
    void mouseEvent(double xpos, double ypos);

    /// @param[in] button The mouse button that was pressed. (See enum MouseButton).
    /// @param[in] action Either PRESS or RELEASE.
    /// @param[in] mods   The modifier keys that are pressed. (MOD_SHIFT, MOD_CTRL, MOD_ALT, MOD_SUPER)
    void mouseButtonEvent(int button, int action, int mods);

    /// Scroll event. 
    /// For mouse wheels, yoffset will be the mouse wheel change.
    /// @param[in] xoffset 
    /// @param[in] yoffset Up is positive and down is negative.
    void scrollEvent(double xoffset, double yoffset);

    /// Drag and drop event.
    /// @param[in] count The number of file paths.
    /// @param[in] paths The array of file paths.
    void dropEvent(int count, const char** paths);

    /// Gets the state of the given key.
    /// @param[in] key The keyboard key to query (like KEY_SPACE).
    /// @return PRESS or RELEASE.
    int getKey(int key);

    /// Gets the mouse button state.
    /// @param[in] button The mouse button to query (like LEFT_MOUSE or MOUSE_BUTTON_0).
    /// @return PRESS or RELEASE.
    int getMouseButton(int button);

    /// Gets the cursor position.
    /// @param[out] xpos The cursor x-position relative to the left of the client area.
    /// @param[out] ypos The cursor y-position relative to the top of the client area.
    void cursorPosition(double* xpos, double* ypos);

    /// Returns the width of the client area.
    int width() const;
    float widthAsFloat() const;

    /// Returns the height of the client area.
    int height() const;
    float heightAsFloat() const;

    /// Returns the aspect ratio of the client area (width/height).
    float aspectRatio() const;

    /// The number of frames so far.
    /// The count will be 1 at the start of the first frame.
    size_t frameCount() const noexcept;

    /// Sets if the app should terminate at the end of this frame.
    void setShouldClose(bool value);

    /// Sets the swap interval.
    /// @param[in] interval 0 to disable vsync and 1 for 60fps.
    void setSwapInterval(int interval);

    /// Sets the cursor visibility.
    /// @param[in] visible True to make the cursor visible; false to hide the cursor.
    void setCursorVisibility(bool visible);

    // Returns true if the cursor is visible.
    bool isCursorVisible() const;

    static App* instance();

public:
    App(const App&) = delete;
    App& operator=(const App&) = delete;
private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};

/// Alternative to using App::instance().
App* app();

/// Convenience function for getting the delta time.
double deltaTime();
}
