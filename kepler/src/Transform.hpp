#pragma once

#include "BaseMath.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace kepler {

class Node;

/// Represents the translation, rotation and scale of an object.
class Transform {
    friend class Node;
public:
    static const glm::vec3 ScaleOne;
public:
    Transform();
    Transform(const glm::vec3& translation, const glm::quat& rotation = glm::quat(), const glm::vec3& scale = ScaleOne);

    inline bool operator==(const Transform& v) const;
    inline bool operator!=(const Transform& v) const;

    inline const glm::vec3& translation() const;
    inline const glm::quat& rotation() const;
    inline const glm::vec3& scale() const;

    /// Sets the translation to the given vector.
    void setTranslation(const glm::vec3& translation);
    /// Sets the translation to the given (x,y,z) position.
    void setTranslation(float x, float y, float z);

    void setScale(const glm::vec3& scale);
    void setScale(float x, float y, float z);

    void setRotation(const glm::quat& rotation);
    void setRotationFromEuler(const glm::vec3& eulerAngles);
    void setRotationFromEuler(float pitch, float yaw, float roll);

    void translate(const glm::vec3& translation);
    void translate(float x, float y, float z);

    /// Scales by the given value.
    void scale(const glm::vec3& scale);
    void scale(float scale);
    void scale(float x, float y, float z);

    void rotate(const glm::quat& rotation);

    /// Returns the 4x4 matrix representation of this transform.
    const glm::mat4& matrix() const;

    void set(const glm::vec3& translation, const glm::quat& rotation, const glm::vec3& scale);

    /// Sets this transform by decomposing the given matrix.
    /// @param[in] matrix The matrix to decompose.
    /// @return True if the matrix can be decomposed; false otherwise.
    bool set(const glm::mat4& matrix);

    // Resets this tranform to the identity transform.
    void loadIdentity();

    void combineWithParent(const Transform& parent);

    /// Decomposes the scale, rotation and translation components of the given matrix.
    /// 
    /// @param[in]  matrix      The matrix to decompose.
    /// @param[out] scale       The vector to copy the scale to.
    /// @param[out] rotation    The quat to copy the rotation to.
    /// @param[out] translation The vector to copy the translation to.
    /// @return True if the decomposition was successful; false otherwise.
    static bool decompose(const glm::mat4& matrix, glm::vec3& scale, glm::quat& rotation, glm::vec3& translation);

private:
    inline void dirty(unsigned char dirtyBits) const;

private: // member variables
    glm::vec3 _translation;
    glm::quat _rotation;
    glm::vec3 _scale;

    // local matrix
    mutable glm::mat4 _matrix;
    mutable unsigned char _dirtyBits;

    // TODO include a flag for being an identity transform?
    static constexpr unsigned char TRANSLATE_DIRTY = 1;
    static constexpr unsigned char ROTATION_DIRTY = 1 << 1;
    static constexpr unsigned char SCALE_DIRTY = 1 << 2;
    static constexpr unsigned char ALL_TRANSFORM_DIRTY = TRANSLATE_DIRTY | ROTATION_DIRTY | SCALE_DIRTY;
};
}

#include "Transform.inl"
