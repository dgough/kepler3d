#pragma once

#include "Base.hpp"
#include "Scene.hpp"
#include "Camera.hpp"

// TODO support a pivot other than origin.

namespace kepler {

/// An orbit camera controller for moving a camera around an object.
/// The roll of the camera is locked.
class OrbitCamera {
public:

    OrbitCamera(); // TODO args. Allow passing in a camera?

    /// Attaches this OrbitCamera to the scene.
    void attach(Scene* scene);

    /// Detaches this OrbitCamera from the scene it is attached to.
    void detach();

    /// Indicates the starting cursor position when moving the camera.
    void start(float x, float y);

    /// Moves the camera using the new cursor position.
    /// Should be called after start().
    /// @param[in] x The new x cursor position.
    /// @param[in] y The new y cursor position.
    void move(float x, float y);

    /// Changes the orbit distance from the target.
    /// @param[in] value Positive values will move away from the target.
    void zoomOut(float value);

    /// Sets the zoom value. The zoom will be clamped to the min zoom.
    /// @param[in] value The new zoom value. 
    void setZoom(float value);

private:
    float _yaw;
    float _pitch;
    float _radius;
    glm::vec2 _start;

    ref<Node> _cameraNode;
    /// The parent node of _cameraNode
    ref<Node> _lookatNode;
    std::weak_ptr<Node> _oldCameraNode;
};
}
