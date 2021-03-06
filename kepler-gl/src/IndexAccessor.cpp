#include "stdafx.h"
#include "IndexAccessor.hpp"
#include "IndexBuffer.hpp"

namespace kepler {
namespace gl {

IndexAccessor::IndexAccessor(const shared_ptr<IndexBuffer>& indexBuffer, GLsizei count, GLenum type, GLintptr offset)
    : _buffer(indexBuffer), _count(count), _type(type), _offset(offset) {
}

shared_ptr<IndexAccessor> IndexAccessor::create(const shared_ptr<IndexBuffer>& indexBuffer, GLsizei count, GLenum type, GLintptr offset) {
    if (indexBuffer == nullptr) {
        return nullptr;
    }
    return std::make_shared<IndexAccessor>(indexBuffer, count, type, offset);
}

void IndexAccessor::bind() {
    _buffer->bind();
}

GLsizei IndexAccessor::count() const {
    return _count;
}

GLenum IndexAccessor::type() const {
    return _type;
}

GLintptr IndexAccessor::offset() const {
    return _offset;
}
}
}
