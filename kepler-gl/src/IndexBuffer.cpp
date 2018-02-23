#include "stdafx.h"
#include "IndexBuffer.hpp"

namespace kepler {
IndexBuffer::IndexBuffer() : _handle(0) {

}
IndexBuffer::~IndexBuffer() noexcept {
    if (_handle != 0) {
        glDeleteBuffers(1, &_handle);
    }
}

ref<IndexBuffer> IndexBuffer::create(GLsizeiptr size, const GLvoid* data, GLenum usage) {
    auto buffer = std::make_shared<IndexBuffer>();
    glGenBuffers(1, &buffer->_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
    return buffer;
}

void IndexBuffer::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _handle);
}
}
