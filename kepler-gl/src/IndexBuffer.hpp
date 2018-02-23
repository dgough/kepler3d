#pragma once

#include <Base.hpp>
#include "BaseGL.hpp"

namespace kepler {

/// Wrapper for a GL_ELEMENT_ARRAY_BUFFER.
class IndexBuffer {
public:
    /// Use IndexBuffer::create() instead.
    IndexBuffer();
    virtual ~IndexBuffer() noexcept;
    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    static ref<IndexBuffer> create(GLsizeiptr size, const GLvoid* data, GLenum usage = GL_STATIC_DRAW);

    /// Binds this buffer.
    void bind();

private:
    IndexBufferHandle _handle;
};
}
