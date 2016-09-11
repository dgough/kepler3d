#include "stdafx.h"
#include "OrbitCamera.hpp"

#include "App.hpp"
#include "BaseMath.hpp"

#include <algorithm>

namespace kepler {

    static constexpr float MIN_ZOOM = 1.f;
    static constexpr float FOV = 45.f;
    static constexpr float NEAR = 0.1f;
    static constexpr float FAR = 1000.f;

    static constexpr float DEFAULT_YAW = 0.f;
    static constexpr float DEFAULT_PITCH = 0.f;
    static constexpr float DEFAULT_RADIUS = 10.f;
    static constexpr float DEFAULT_ASPECT_RATIO = 4.f / 3.f;

    static inline float getDefaultAspectRatio() {
        return app() ? app()->getAspectRatio() : DEFAULT_ASPECT_RATIO;
    }

    OrbitCamera::OrbitCamera() : _yaw(DEFAULT_YAW), _pitch(DEFAULT_PITCH), _radius(DEFAULT_RADIUS) {
    }

    void OrbitCamera::attach(SceneRef scene) {
        if (!scene) {
            return;
        }

        if (scene->getActiveCamera()) {
            _oldCameraNode = scene->getActiveCamera()->getNode();
        }

        if (!_cameraNode) {
            _cameraNode = Node::create("orbitCamera");
            _cameraNode->translateZ(_radius);

            // don't require app
            auto camera = Camera::createPerspective(FOV, getDefaultAspectRatio(), NEAR, FAR);
            _cameraNode->addComponent(camera);

            _lookatNode = Node::create("orbitCameraLookAt");

            _lookatNode->addNode(_cameraNode);
        }
        scene->addNode(_lookatNode);
        scene->setActiveCamera(_cameraNode->getComponent<Camera>()); // save ref to camera?
    }

    void OrbitCamera::detach() {
        if (_lookatNode) {
            if (auto scene = _lookatNode->getScene()) {
                scene->setActiveCamera(nullptr);
                scene->removeChild(_lookatNode);
            }
        }
        if (auto n = _oldCameraNode.lock()) {
            if (auto scene = n->getScene()) {
                scene->setActiveCamera(n->getComponent<Camera>());
            }
        }
        _oldCameraNode.reset();
    }

    void OrbitCamera::start(float x, float y) {
        _start.x = x;
        _start.y = y;
    }

    void OrbitCamera::move(float x, float y) {
        if (!_lookatNode) {
            return;
        }
        auto deltaX = x - _start.x;
        auto deltaY = y - _start.y;
        _start = glm::vec2(x, y);
        _pitch = _pitch - static_cast<float>(glm::radians(deltaY * 0.5f));
        _yaw = _yaw - static_cast<float>(glm::radians(deltaX * 0.5f));

        _pitch = std::max(-PI_OVER_2, std::min(_pitch, PI_OVER_2));

        _lookatNode->setRotation(glm::quat());
        _lookatNode->rotateX(_pitch);
        _lookatNode->rotateY(_yaw);
    }

    void OrbitCamera::zoomOut(float value) {
        setZoom(_radius + value);
    }

    void OrbitCamera::setZoom(float value) {
        if (_cameraNode) {
            _radius = std::max(value, MIN_ZOOM);
            _cameraNode->setTranslation(0.f, 0.f, _radius);
        }
    }
}
