#include "stdafx.h"
#include "VertexBuffer.hpp"

namespace kepler {
namespace gl {

VertexBuffer::VertexBuffer() : _handle(0) {

}
VertexBuffer::~VertexBuffer() noexcept {
    if (_handle != 0) {
        glDeleteBuffers(1, &_handle);
    }
}

ref<VertexBuffer> VertexBuffer::create(GLsizeiptr size, const GLvoid* data, GLenum usage) {
    auto buffer = std::make_shared<VertexBuffer>();
    glGenBuffers(1, &buffer->_handle);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->_handle);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    return buffer;
}

void VertexBuffer::bind() const noexcept {
    glBindBuffer(GL_ARRAY_BUFFER, _handle);
}
}
}
