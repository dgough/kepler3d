#pragma once

#include "Base.hpp"

#pragma warning(push)
// Suppress "unreferenced formal parameter" warning
#pragma warning(disable : 4100)

namespace kepler {

    /// Application delegate.
    /// 
    /// The purpose of this class is to allow apps to change which class 
    /// will receive calls to the main life-cycle methods.
    /// Use App#setDelegate() to set the current AppDelegate.
    /// 
    /// @see App for documentation for most of these methods.
    class AppDelegate {
    public:
        AppDelegate() {}
        virtual ~AppDelegate() noexcept = default;

        virtual void start() {}
        virtual void stop() {}
        virtual void update() = 0;
        virtual void render() = 0;
        virtual void keyEvent(int key, int scancode, int action, int mods) {}
        virtual void mouseEvent(double xpos, double ypos) {}
        virtual void mouseButtonEvent(int button, int action, int mods) {}
        virtual void scrollEvent(double xoffset, double yoffset) {}
        /// Drag and drop event.
        virtual void dropEvent(int count, const char** paths) {}

    private:
        AppDelegate(const AppDelegate&) = delete;
        AppDelegate& operator=(const AppDelegate&) = delete;
    private:

    };
}
#pragma warning(pop)
