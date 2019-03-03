#pragma once

#include <Buffer.hpp>

namespace kepler {
namespace gl {

class VertexBuffer : public Buffer<GL_ARRAY_BUFFER> {
public:
    /// Use VertexBuffer::create() instead.
    VertexBuffer() = default;
    VertexBuffer(GLsizeiptr size, const GLvoid* data, GLenum usage = GL_STATIC_DRAW) : Buffer<GL_ARRAY_BUFFER>(size, data, usage) {}

    // Creates a shared_ptr to a new VertexBuffer
    static shared_ptr<VertexBuffer> create(GLsizeiptr size, const GLvoid* data, GLenum usage = GL_STATIC_DRAW) {
        return std::make_shared<VertexBuffer>(size, data, usage);
    }
};

static_assert(sizeof(VertexBuffer) == sizeof(BufferHandle), "Ensure no vtable");

} // namespace gl
} // namespace kepler
