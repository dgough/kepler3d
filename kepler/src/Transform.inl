#include "Transform.hpp"

namespace kepler {

    inline bool Transform::operator==(const Transform& v) const {
        return _translation == v._translation
            && _rotation == v._rotation
            && _scale == v._scale;
    }

    inline bool Transform::operator!=(const Transform& v) const {
        return !(*this == v);
    }

    inline const glm::vec3& Transform::getTranslation() const {
        return _translation;
    }

    inline const glm::quat& Transform::getRotation() const {
        return _rotation;
    }

    inline const glm::vec3& Transform::getScale() const {
        return _scale;
    }

    inline void Transform::dirty(unsigned char dirtyBits) const {
        _dirtyBits |= dirtyBits;
    }
}
