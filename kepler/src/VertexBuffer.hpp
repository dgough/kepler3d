#pragma once

#include "Base.hpp"
#include "BaseGL.hpp"

namespace kepler {

    /// Wrapper for a Vertex Buffer Object (GL_ARRAY_BUFFER).
    class VertexBuffer {
        ALLOW_MAKE_SHARED(VertexBuffer);
    public:
        virtual ~VertexBuffer() noexcept;

        /// Creates a VertexBuffer.
        /// 
        /// @param[in] size Size of the data.
        /// @param[in] data Pointer to the vertex data.
        /// @param[in] usage GL_STATIC_DRAW, GL_DYNAMIC_DRAW...
        static ref<VertexBuffer> create(GLsizeiptr size, const GLvoid* data, GLenum usage = GL_STATIC_DRAW);
        
        /// Binds this buffer.
        void bind() const noexcept;
    public:
        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;
    private:
        VertexBuffer();

    private:
        VertexBufferHandle _handle;
    };
}
