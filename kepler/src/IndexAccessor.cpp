#include "stdafx.h"
#include "IndexAccessor.hpp"
#include "IndexBuffer.hpp"

namespace kepler {

    IndexAccessor::IndexAccessor(IndexBufferRef indexBuffer, GLsizei count, GLenum type, GLintptr offset)
        : _buffer(indexBuffer), _count(count), _type(type), _offset(offset) {
    }

    IndexAccessorRef IndexAccessor::create(IndexBufferRef indexBuffer, GLsizei count, GLenum type, GLintptr offset) {
        if (indexBuffer == nullptr) {
            return nullptr;
        }
        return std::make_shared<IndexAccessor>(indexBuffer, count, type, offset);
    }

    void IndexAccessor::bind() {
        _buffer->bind();
    }

    GLsizei IndexAccessor::getCount() const {
        return _count;
    }

    GLenum IndexAccessor::getType() const {
        return _type;
    }

    GLintptr IndexAccessor::getOffset() const {
        return _offset;
    }
}
