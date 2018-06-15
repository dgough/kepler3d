#pragma once

#include <BaseGL.hpp>
#include <OpenGL.hpp>

namespace kepler {
namespace gl {

class IndexAccessor final {
public:
    enum Type {
        UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
        UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
        UNSIGNED_INT = GL_UNSIGNED_INT
    };
    // Use IndexAccessor::create()
    IndexAccessor(const ref<IndexBuffer>& indexBuffer, GLsizei count, GLenum type, GLintptr offset);
    IndexAccessor(const IndexAccessor&) = delete;
    IndexAccessor& operator=(const IndexAccessor&) = delete;

    /// Creates a IndexAccessor.
    /// 
    /// @param[in] indexBuffer The index buffer that contains the indices.
    /// @param[in] count       The number of elements.
    /// @param[in] type        The type of the values. Must be UNSIGNED_BYTE, UNSIGNED_SHORT or UNSIGNED_INT.
    /// @param[in] offset      The offset into the buffer.
    /// @return Shared pointer to newly created IndexAccessor.
    static ref<IndexAccessor> create(const ref<IndexBuffer>& indexBuffer, GLsizei count, GLenum type, GLintptr offset);

    void bind();

    GLsizei count() const;
    GLenum type() const;
    GLintptr offset() const;

private:
    ref<IndexBuffer> _buffer;
    GLsizei _count;
    GLenum _type;
    GLintptr _offset;
};
}
}
