#pragma once

#include "Base.hpp"
#include "BaseMath.hpp"

namespace kepler {

/// First person flying camera controller.
class FirstPersonController {
public:
    /// Constructor.
    ///
    /// @param[in] fov The field of view Y.
    /// @param[in] width  Screen width. Used to calculate the aspect ratio.
    /// @param[in] height Screen height. Used to calculate the aspect ratio.
    /// @param[in] near Near plane.
    /// @param[in] far Far plane.
    FirstPersonController(float fov, float width, float height, float near, float far);

    /// Returns the root camera Node.
    shared_ptr<Node> rootNode() const;

    /// Returns the Camera.
    shared_ptr<Camera> camera() const;

    /// Sets the position of the camera.
    /// @param[in] position The position to move to.
    void setPosition(const vec3& position);

    /// Moves the camera forward in the direction that it is pointing.
    /// (Fly mode)
    /// @param[in] value The amount to move.
    void moveForward(float value);

    /// Moves the camera in the opposite direction that it is pointing.
    /// @param[in] value The amount to move.
    void moveBackward(float value);

    /// Strafes that camera left, which is perpendicular to the direction it is facing.
    /// @param[in] value The amount to move.
    void moveLeft(float value);

    /// Strafes that camera right, which is perpendicular to the direction it is facing.
    /// @param[in] value The amount to move.
    void moveRight(float value);

    /// Moves the camera up (positive y).
    /// @param[in] value The amount to move.
    void moveUp(float value);

    /// Moves the camera down.
    /// @param[in] value The amount to move.
    void moveDown(float value);

    /// Rotates the camera in place in order to change the direction it is looking.
    /// @param[in] yaw Rotates the camera around the yaw axis in radians. Positive looks right, negative looks left.
    /// @param[in] pitch Rotates the camera around the ptich axis in radians. Positive looks up, negative looks down.
    void rotate(float yaw, float pitch);

    /// Returns true if the mouse Y axis is inverted.
    /// @return True if the mouse is inverted; false otherwise.
    bool isInvertedY() const;

    /// Enables or disables inverted mouse.
    /// @param[in] enable True to invert the mouse.
    void setInvertY(bool enable);

private:
    FirstPersonController(const FirstPersonController&) = delete;
    FirstPersonController& operator=(const FirstPersonController&) = delete;

    shared_ptr<Node> _root;
    shared_ptr<Node> _pitchNode;
    float _invertY;
    float _pitch;
};
}
