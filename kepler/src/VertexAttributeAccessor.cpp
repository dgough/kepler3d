#include "stdafx.h"
#include "VertexAttributeAccessor.hpp"
#include "VertexBuffer.hpp"

namespace kepler {
    VertexAttributeAccessor::VertexAttributeAccessor() : _count(0) {

    }

    VertexAttributeAccessor::~VertexAttributeAccessor() noexcept {

    }

    VertexAttributeAccessorRef VertexAttributeAccessor::create(VertexBufferRef vbo,
        GLint componentSize, GLenum type, GLboolean normalized, GLsizei stride, GLintptr offset, GLsizei count) {

        auto p = MAKE_SHARED(VertexAttributeAccessor);
        // TODO clean this up
        p->_vbo = vbo;
        p->_componentSize = componentSize;
        p->_type = type;
        p->_normalized = normalized;
        p->_stride = stride;
        p->_offset = offset;
        p->_count = count;
        return p;
    }

    void VertexAttributeAccessor::bind(GLuint location) {
        _vbo->bind();
        glVertexAttribPointer(location, _componentSize, _type, _normalized, _stride, (GLvoid *)_offset);
    }

    GLsizei VertexAttributeAccessor::getCount() const {
        return _count;
    }

    void VertexAttributeAccessor::setCount(GLsizei count) {
        _count = count;
    }
}
