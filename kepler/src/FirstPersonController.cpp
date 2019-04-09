#include "stdafx.h"
#include "FirstPersonController.hpp"

#include "Node.hpp"
#include "Camera.hpp"

namespace kepler {

static constexpr float MAX_PITCH = 89.0f * PI / 180.0f;
static constexpr float MIN_PITCH = -MAX_PITCH;

FirstPersonController::FirstPersonController(float fov, float width, float height, float near, float far)
    : _root(Node::create()), _invertY(-1.0f), _pitch(0.0f) {
    _pitchNode = _root->createChild();
    auto camera = Camera::createPerspectiveFov(fov, width, height, near, far);
    _pitchNode->addComponent(camera);
}

shared_ptr<Node> FirstPersonController::rootNode() const {
    return _root;
}

shared_ptr<Camera> FirstPersonController::camera() const {
    return _pitchNode->component<Camera>();
}

void FirstPersonController::setPosition(const vec3& position) {
    // Assumes _root is actually a root node
    _root->setTranslation(position);
}

void FirstPersonController::moveForward(float value) {
    _root->translate(glm::normalize(_pitchNode->forwardVectorWorld()) * value);
}

void FirstPersonController::moveBackward(float value) {
    moveForward(-value);
}

void FirstPersonController::moveLeft(float value) {
    moveRight(-value);
}

void FirstPersonController::moveRight(float value) {
    vec3 v = glm::normalize(glm::cross(_pitchNode->forwardVectorWorld(), vec3(0, 1, 0)));
    _root->translate(v * value);
}

void FirstPersonController::moveUp(float value) {
    _root->translate(0, value, 0);
}

void FirstPersonController::moveDown(float value) {
    _root->translate(0, -value, 0);
}

void FirstPersonController::rotate(float yaw, float pitch) {
    _root->rotateY(-yaw);

    _pitch += pitch * _invertY;
    _pitch = std::max(MIN_PITCH, std::min(_pitch, MAX_PITCH));
    _pitchNode->setRotationFromEuler(_pitch, 0.0f, 0.0f);
}

bool FirstPersonController::isInvertedY() const {
    return _invertY != 1.0f;
}

void FirstPersonController::setInvertY(bool enable) {
    _invertY = enable ? -1.0f : 1.0f;
}
}
