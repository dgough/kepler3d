#pragma once

#include <Buffer.hpp>

namespace kepler {
namespace gl {

class IndexBuffer : public Buffer<GL_ELEMENT_ARRAY_BUFFER> {
public:
    /// Use IndexBuffer::create() instead.
    IndexBuffer() = default;
    IndexBuffer(GLsizeiptr size, const GLvoid* data, GLenum usage = GL_STATIC_DRAW) : Buffer<GL_ELEMENT_ARRAY_BUFFER>(size, data, usage) {}

    // Creates a shared_ptr to a new IndexBuffer
    static shared_ptr<IndexBuffer> create(GLsizeiptr size, const GLvoid* data, GLenum usage = GL_STATIC_DRAW) {
        return std::make_shared<IndexBuffer>(size, data, usage);
    }
};

static_assert(sizeof(IndexBuffer) == sizeof(BufferHandle), "Ensure no vtable");

} // namespace gl
} // namespace kepler
