#pragma once

#include <BaseGL.hpp>
#include <OpenGL.hpp>

namespace kepler {
namespace gl {

class VertexAttributeAccessor final {
public:
    VertexAttributeAccessor(const ref<VertexBuffer>& vbo,
        GLint componentSize, GLenum type, GLboolean normalized, GLsizei stride, GLintptr offset, GLsizei count);

    // TODO this class probably doesn't need to be ref counted since it is immutable.
    static ref<VertexAttributeAccessor> create(const ref<VertexBuffer>& vbo,
        GLint componentSize, GLenum type, GLboolean normalized, GLsizei stride, GLintptr offset, GLsizei count);

    void bind(GLuint location) const noexcept; // TODO should this be GLint so we can detect negative numbers?

    GLsizei count() const;

private:
    ref<VertexBuffer> _vbo;
    GLint _componentSize;
    GLenum _type;
    GLboolean _normalized;
    GLsizei _stride;
    GLintptr _offset;
    GLsizei _count;
};
}
}