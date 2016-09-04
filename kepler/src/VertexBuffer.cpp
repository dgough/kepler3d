#include "stdafx.h"
#include "VertexBuffer.hpp"

namespace kepler {
    VertexBuffer::VertexBuffer() : _handle(0) {

    }
    VertexBuffer::~VertexBuffer() noexcept {
        if (_handle != 0) {
            glDeleteBuffers(1, &_handle);
        }
    }

    VertexBufferRef VertexBuffer::create(GLsizeiptr size, const GLvoid* data, GLenum usage) {
        auto buffer = MAKE_SHARED(VertexBuffer);
        glGenBuffers(1, &buffer->_handle);
        glBindBuffer(GL_ARRAY_BUFFER, buffer->_handle);
        glBufferData(GL_ARRAY_BUFFER, size, data, usage);
        return buffer;
    }

    void VertexBuffer::bind() const noexcept {
        glBindBuffer(GL_ARRAY_BUFFER, _handle);
    }
}
