#pragma once

#include "Node.hpp"
#include "BaseMath.hpp"

namespace kepler {

/// The camera class represents an orthographic or perspective camera.
class Camera : public Component, public Node::Listener, public std::enable_shared_from_this<Camera> {
public:

    /// Camera types.
    enum class Type {
        PERSPECTIVE = 1,
        ORTHOGRAPHIC = 2
    };

    /// Perspective camera constructor. Use Camera::createPerspective() instead.
    Camera(float fov, float aspectRatio, float near, float far);
    /// Orthographic camera constructor. Use Camera::createOrthographic() instead.
    Camera(float zoomX, float zoomY, float aspectRatio, float near, float far);
    virtual ~Camera() noexcept;

    /// Creates a perspective camera.
    /// @param[in] fov         Field of view in degrees.
    /// @param[in] aspectRatio Aspect ratio
    /// @param[in] near        Near plane distance.
    /// @param[in] far         Far plane distance.
    static shared_ptr<Camera> createPerspective(float fov, float aspectRatio, float near, float far);

    /// Creates a perspective camera with a FOV based on the given width and height.
    /// @param[in] fov         Field of view in degrees.
    /// @param[in] width       Width of the client display area.
    /// @param[in] height      Height of the client display area.
    /// @param[in] near        Near plane distance.
    /// @param[in] far         Far plane distance.
    static shared_ptr<Camera> createPerspectiveFov(float fov, float width, float height, float near, float far);

    // TODO ortho doesn't need aspect ratio
    static shared_ptr<Camera> createOrthographic(float zoomX, float zoomY, float aspectRatio, float near, float far);

    void onNodeChanged(const shared_ptr<Node>& oldNode, const shared_ptr<Node>& newNode) override;
    const std::string& typeName() const override;

    Camera::Type cameraType() const;

    void transformChanged(const Node* node) override;

    const mat4& viewMatrix() const;
    const mat4& projectionMatrix() const;
    const mat4& viewProjectionMatrix() const;

    /// Returns field of view in degrees.
    float fov() const noexcept;
    float aspectRatio() const noexcept;
    float near() const noexcept;
    float far() const noexcept;
    float zoomX() const noexcept;
    float zoomY() const noexcept;

public:
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;

private:
    Camera::Type _type;
    float _fov = 45.0f;
    float _aspectRatio;
    float _near;
    float _far;
    float _zoomX = 0.0f;
    float _zoomY = 0.0f;

    mutable mat4 _view;
    mutable mat4 _projection;
    mutable mat4 _viewProjection;
    mutable unsigned char _dirtyBits;

    static constexpr unsigned char WORLD_DIRTY = 1;
};
}
