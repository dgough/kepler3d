#include "stdafx.h"
#include "Camera.hpp"
#include "Node.hpp"
#include "Performance.hpp"

namespace kepler {

    static constexpr unsigned char VIEW_DIRTY = 1;
    static constexpr unsigned char PROJ_DIRTY = 2;
    static constexpr unsigned char VIEW_PROJ_DIRTY = 4;
    static constexpr unsigned char INV_VIEW_DIRTY = 8;
    static constexpr unsigned char INV_VIEW_PROJ_DIRTY = 16;
    static constexpr unsigned char BOUNDS_DIRTY = 32;
    static constexpr unsigned char ALL_DIRTY = (VIEW_DIRTY | PROJ_DIRTY | VIEW_PROJ_DIRTY | INV_VIEW_DIRTY | INV_VIEW_PROJ_DIRTY | BOUNDS_DIRTY);
    static constexpr unsigned char TRANSFORM_CHANGE = ALL_DIRTY & ~PROJ_DIRTY;

    static std::string _typeName("Camera");

    Camera::Camera(float fov, float aspectRatio, float near, float far)
        : _type(Type::PERSPECTIVE), _fov(fov), _aspectRatio(aspectRatio), _near(near), _far(far) {
        _dirtyBits |= ALL_DIRTY;
    }

    Camera::Camera(float zoomX, float zoomY, float aspectRatio, float near, float far) :
        _type(Type::ORTHOGRAPHIC), _aspectRatio(aspectRatio), _near(near), _far(far), _zoomX(zoomX), _zoomY(zoomY) {
        _dirtyBits |= ALL_DIRTY;
    }

    Camera::~Camera() noexcept {
    }

    ref<Camera> Camera::createPerspective(float fov, float aspectRatio, float near, float far) {
        return std::make_shared<Camera>(fov, aspectRatio, near, far);
    }

    ref<Camera> Camera::createPerspectiveFov(float fov, float width, float height, float near, float far) {
        return createPerspective(fov, width / height, near, far);
    }

    ref<Camera> Camera::createOrthographic(float zoomX, float zoomY, float aspectRatio, float near, float far) {
        return std::make_shared<Camera>(zoomX, zoomY, aspectRatio, near, far);
    }

    void Camera::onNodeChanged(const ref<Node>& oldNode, const ref<Node>& newNode) {
        if (oldNode) {
            oldNode->removeListener(this);
        }
        if (newNode) {
            newNode->addListener(shared_from_this());
        }
    }

    const std::string& Camera::typeName() const {
        return _typeName;
    }

    Camera::Type Camera::cameraType() const {
        return _type;
    }

    void Camera::transformChanged(const Node*) {
        _dirtyBits |= TRANSFORM_CHANGE;
    }

    const glm::mat4& Camera::viewMatrix() const {
        if (_dirtyBits & VIEW_DIRTY) {
            if (auto node = _node.lock()) {
                _view = glm::inverse(node->worldMatrix());
            }
            else {
                _view = IDENTITY_MATRIX;
            }
            _dirtyBits &= ~VIEW_DIRTY;
        }
        return _view;
    }

    const glm::mat4& Camera::projectionMatrix() const {
        if (_dirtyBits & PROJ_DIRTY) {
            if (_type == Type::PERSPECTIVE) {
                // TODO check GLM_FORCE_RADIANS?
                _projection = glm::perspective(_fov, _aspectRatio, _near, _far);
            }
            else if (_type == Type::ORTHOGRAPHIC) {
                float halfWidth = _zoomX * 0.5f;
                float halfHeight = _zoomY * 0.5f;
                _projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, _near, _far);
            }
            _dirtyBits &= ~PROJ_DIRTY;
        }
        return _projection;
    }

    const glm::mat4& Camera::viewProjectionMatrix() const {
        if (_dirtyBits & VIEW_PROJ_DIRTY) {
            _viewProjection = projectionMatrix() * viewMatrix();
            _dirtyBits &= ~VIEW_PROJ_DIRTY;
        }
        return _viewProjection;
    }
    float Camera::fov() const noexcept {
        return _fov;
    }
    float Camera::aspectRatio() const noexcept {
        return _aspectRatio;
    }
    float Camera::near() const noexcept {
        return _near;
    }
    float Camera::far() const noexcept {
        return _far;
    }
    float Camera::zoomX() const noexcept {
        return _zoomX;
    }
    float Camera::zoomY() const noexcept {
        return _zoomY;
    }
}
