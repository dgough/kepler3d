#pragma once

#include "Base.hpp"
#include "BaseGL.hpp"

namespace kepler {

    class VertexAttributeAccessor final {
    public:
        VertexAttributeAccessor(VertexBufferRef vbo,
            GLint componentSize, GLenum type, GLboolean normalized, GLsizei stride, GLintptr offset, GLsizei count);

        // TODO this class probably doesn't need to be ref counted since it is immutable.
        static VertexAttributeAccessorRef create(VertexBufferRef vbo,
            GLint componentSize, GLenum type, GLboolean normalized, GLsizei stride, GLintptr offset, GLsizei count);

        void bind(GLuint location) const noexcept; // TODO should this be GLint so we can detect negative numbers?

        GLsizei count() const;

    private:
        VertexBufferRef _vbo;
        GLint _componentSize;
        GLenum _type;
        GLboolean _normalized;
        GLsizei _stride;
        GLintptr _offset;
        GLsizei _count;
    };
}
