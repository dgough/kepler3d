#pragma once

#include <BaseGL.hpp>
#include <OpenGL.hpp>

namespace kepler {
namespace gl {

/// Wrapper for VAO.
class VertexAttributeBinding final {
public:
    VertexAttributeBinding() = default;
    explicit VertexAttributeBinding(GLuint handle);

    VertexAttributeBinding(const MeshPrimitive& meshPrim, const Technique& technique, const Effect& effect);

    ~VertexAttributeBinding() noexcept;

    // Not copyable
    VertexAttributeBinding(const VertexAttributeBinding&) = delete;
    VertexAttributeBinding& operator=(const VertexAttributeBinding&) = delete;

    // Movable
    VertexAttributeBinding(VertexAttributeBinding&& other) noexcept;
    VertexAttributeBinding& operator=(VertexAttributeBinding&& other) noexcept;

    void bind() {
        glBindVertexArray(_handle);
    }

    void unbind() {
        glBindVertexArray(0);
    }

    void destroy();

    explicit operator bool() const noexcept {
        return _handle != 0;
    }

private:
    GLuint _handle = 0;
};

} // namespace gl
} // namespace kepler
