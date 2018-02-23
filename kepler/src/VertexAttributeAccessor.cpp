#include "stdafx.h"
#include "VertexAttributeAccessor.hpp"
#include "VertexBuffer.hpp"

namespace kepler {
VertexAttributeAccessor::VertexAttributeAccessor(ref<VertexBuffer> vbo,
    GLint componentSize, GLenum type, GLboolean normalized, GLsizei stride, GLintptr offset, GLsizei count)
    : _vbo(vbo), _componentSize(componentSize), _type(type), _normalized(normalized), _stride(stride), _offset(offset), _count(count) {
}

ref<VertexAttributeAccessor> VertexAttributeAccessor::create(ref<VertexBuffer> vbo,
    GLint componentSize, GLenum type, GLboolean normalized, GLsizei stride, GLintptr offset, GLsizei count) {
    return std::make_shared<VertexAttributeAccessor>(vbo, componentSize, type, normalized, stride, offset, count);
}

void VertexAttributeAccessor::bind(GLuint location) const noexcept {
    _vbo->bind();
    glVertexAttribPointer(location, _componentSize, _type, _normalized, _stride, (GLvoid *)_offset);
}

GLsizei VertexAttributeAccessor::count() const {
    return _count;
}
}
