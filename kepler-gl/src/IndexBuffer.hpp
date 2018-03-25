#pragma once

#include <BaseGL.hpp>
#include <OpenGL.hpp>

namespace kepler {
namespace gl {


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
}
