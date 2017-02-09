#pragma once

#include "Base.hpp"
#include "BaseGL.hpp"

namespace kepler {

    /// Wrapper for a GL_ELEMENT_ARRAY_BUFFER.
    class IndexBuffer {
        ALLOW_MAKE_SHARED(IndexBuffer);
    public:
        virtual ~IndexBuffer() noexcept;
        static ref<IndexBuffer> create(GLsizeiptr size, const GLvoid* data, GLenum usage = GL_STATIC_DRAW);

        /// Binds this buffer.
        void bind();

    public:
        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer& operator=(const IndexBuffer&) = delete;
    private:
        IndexBuffer();
    private:
        IndexBufferHandle _handle;
    };
}
