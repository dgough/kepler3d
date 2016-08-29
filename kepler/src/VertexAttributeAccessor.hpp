#pragma once

#include "Base.hpp"
#include "BaseGL.hpp"

namespace kepler {

    class VertexAttributeAccessor {
        ALLOW_MAKE_SHARED(VertexAttributeAccessor);
    public:
        VertexAttributeAccessor();
        virtual ~VertexAttributeAccessor() noexcept;


        static VertexAttributeAccessorRef create(VertexBufferRef vbo,
            GLint componentSize, GLenum type, GLboolean normalized, GLsizei stride, GLintptr offset, GLsizei count);

        void bind(GLuint location); // TODO should this be GLint so we can detect negative numbers?

        GLsizei getCount() const;
        void setCount(GLsizei count);

    public:
        VertexAttributeAccessor(const VertexAttributeAccessor&) = delete;
        VertexAttributeAccessor& operator=(const VertexAttributeAccessor&) = delete;
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
