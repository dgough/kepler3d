#include "stdafx.h"
#include "Transform.hpp"

#include <glm/gtx/transform.hpp>
//#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace kepler {

const glm::vec3 Transform::ScaleOne(1);

Transform::Transform() : _scale(ScaleOne), _dirtyBits(0) {
}

Transform::Transform(const glm::vec3& translation, const glm::quat& rotation, const glm::vec3& scale)
    : _translation(translation), _rotation(rotation), _scale(scale), _dirtyBits(ALL_TRANSFORM_DIRTY) {
}

void Transform::setTranslation(const glm::vec3& translation) {
    dirty(TRANSLATE_DIRTY);
    _translation = translation;
}

void Transform::setTranslation(float x, float y, float z) {
    dirty(TRANSLATE_DIRTY);
    _translation.x = x;
    _translation.y = y;
    _translation.z = z;
}

void Transform::setScale(const glm::vec3& scale) {
    dirty(SCALE_DIRTY);
    _scale = scale;
}

void Transform::setScale(float x, float y, float z) {
    dirty(SCALE_DIRTY);
    _scale.x = x;
    _scale.y = y;
    _scale.z = z;
}

void Transform::setRotation(const glm::quat& rotation) {
    dirty(ROTATION_DIRTY);
    _rotation = rotation;
}

void Transform::setRotationFromEuler(const glm::vec3& eulerAngles) {
    setRotation(glm::quat(eulerAngles));
}

void Transform::setRotationFromEuler(float pitch, float yaw, float roll) {
    setRotationFromEuler(glm::vec3(pitch, yaw, roll));
}

void Transform::translate(const glm::vec3& translation) {
    dirty(TRANSLATE_DIRTY);
    _translation += translation;
}

void Transform::translate(float x, float y, float z) {
    dirty(TRANSLATE_DIRTY);
    _translation.x += x;
    _translation.y += y;
    _translation.z += z;
}

void Transform::scale(const glm::vec3& scale) {
    dirty(SCALE_DIRTY);
    _scale *= scale;
}

void Transform::scale(float scale) {
    dirty(SCALE_DIRTY);
    _scale *= scale;
}

void Transform::scale(float x, float y, float z) {
    dirty(SCALE_DIRTY);
    _scale.x *= x;
    _scale.y *= y;
    _scale.z *= z;
}

void Transform::rotate(const glm::quat& rotation) {
    dirty(ROTATION_DIRTY);
    _rotation = rotation * _rotation;
}

const glm::mat4& Transform::matrix() const {
    if ((_dirtyBits & ALL_TRANSFORM_DIRTY) == 0) {
        return _matrix;
    }

    _matrix = glm::translate(_translation);
    if (_rotation != glm::quat()) {
        _matrix = _matrix * glm::mat4_cast(_rotation);
    }
    if (_scale != ScaleOne) {
        _matrix = glm::scale(_matrix, _scale);
    }
    _dirtyBits &= ~ALL_TRANSFORM_DIRTY;
    return _matrix;
}

void Transform::set(const glm::vec3& translation, const glm::quat& rotation, const glm::vec3& scale) {
    dirty(ALL_TRANSFORM_DIRTY);
    _translation = translation;
    _rotation = rotation;
    _scale = scale;
}

bool Transform::set(const glm::mat4& matrix) {
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    if (decompose(matrix, scale, rotation, translation)) {
        _scale = scale;
        _rotation = rotation;
        _translation = translation;
        _matrix = matrix;
        _dirtyBits &= ~ALL_TRANSFORM_DIRTY;
        return true;
    }
    return false;
}

void Transform::loadIdentity() {
    _translation = glm::vec3();
    _rotation = glm::quat();
    _scale = ScaleOne;
    _matrix = glm::mat4();
    _dirtyBits = 0;
}

void Transform::combineWithParent(const Transform& parent) {
    dirty(ALL_TRANSFORM_DIRTY);
    _scale *= parent._scale;
    _rotation = parent._rotation * _rotation;

    _translation *= parent._scale;
    _translation = parent._rotation * _translation;
    _translation += parent._translation;
}

bool Transform::decompose(const glm::mat4& matrix, glm::vec3& scale, glm::quat& rotation, glm::vec3& translation) {
    // This was copied from gameplay3d
    auto m = glm::value_ptr(matrix);

    // Extract the translation.
    translation.x = m[12];
    translation.y = m[13];
    translation.z = m[14];

    // Extract the scale.
    // This is simply the length of each axis (row/column) in the matrix.
    glm::vec3 xaxis(m[0], m[1], m[2]);
    float scaleX = glm::length(xaxis);

    glm::vec3 yaxis(m[4], m[5], m[6]);
    float scaleY = glm::length(yaxis);

    glm::vec3 zaxis(m[8], m[9], m[10]);
    float scaleZ = glm::length(zaxis);

    // Determine if we have a negative scale (true if determinant is less than zero).
    // In this case, we simply negate a single axis of the scale.
    float det = glm::determinant(matrix);
    if (det < 0) {
        scaleZ = -scaleZ;
    }

    scale.x = scaleX;
    scale.y = scaleY;
    scale.z = scaleZ;

    static constexpr float MATH_TOLERANCE = 2e-37f;
    // Scale too close to zero, can't decompose rotation.
    if (scaleX < MATH_TOLERANCE || scaleY < MATH_TOLERANCE || fabs(scaleZ) < MATH_TOLERANCE) {
        return false;
    }

    // Factor the scale out of the matrix axes.
    float rn = 1.0f / scaleX;
    xaxis.x *= rn;
    xaxis.y *= rn;
    xaxis.z *= rn;

    rn = 1.0f / scaleY;
    yaxis.x *= rn;
    yaxis.y *= rn;
    yaxis.z *= rn;

    rn = 1.0f / scaleZ;
    zaxis.x *= rn;
    zaxis.y *= rn;
    zaxis.z *= rn;

    // Now calculate the rotation from the resulting matrix (axes).
    float trace = xaxis.x + yaxis.y + zaxis.z + 1.0f;

    if (trace > 1.0f) {
        float s = 0.5f / sqrt(trace);
        rotation.w = 0.25f / s;
        rotation.x = (yaxis.z - zaxis.y) * s;
        rotation.y = (zaxis.x - xaxis.z) * s;
        rotation.z = (xaxis.y - yaxis.x) * s;
    }
    else {
        // Note: since xaxis, yaxis, and zaxis are normalized, 
        // we will never divide by zero in the code below.
        if (xaxis.x > yaxis.y && xaxis.x > zaxis.z) {
            float s = 0.5f / sqrt(1.0f + xaxis.x - yaxis.y - zaxis.z);
            rotation.w = (yaxis.z - zaxis.y) * s;
            rotation.x = 0.25f / s;
            rotation.y = (yaxis.x + xaxis.y) * s;
            rotation.z = (zaxis.x + xaxis.z) * s;
        }
        else if (yaxis.y > zaxis.z) {
            float s = 0.5f / sqrt(1.0f + yaxis.y - xaxis.x - zaxis.z);
            rotation.w = (zaxis.x - xaxis.z) * s;
            rotation.x = (yaxis.x + xaxis.y) * s;
            rotation.y = 0.25f / s;
            rotation.z = (zaxis.y + yaxis.z) * s;
        }
        else {
            float s = 0.5f / sqrt(1.0f + zaxis.z - xaxis.x - yaxis.y);
            rotation.w = (xaxis.y - yaxis.x) * s;
            rotation.x = (zaxis.x + xaxis.z) * s;
            rotation.y = (zaxis.y + yaxis.z) * s;
            rotation.z = 0.25f / s;
        }
    }
    return true;
}
}
