#pragma once

#include "Base.hpp"
#include "BaseGL.hpp"

namespace kepler {

    class IndexAccessor final {
    public:
        enum Type {
            UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
            UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
            UNSIGNED_INT = GL_UNSIGNED_INT
        };
        // Use IndexAccessor::create()
        IndexAccessor(IndexBufferRef indexBuffer, GLsizei count, GLenum type, GLintptr offset);

        /// Creates a IndexAccessor.
        /// 
        /// @param[in] indexBuffer The index buffer that contains the indices.
        /// @param[in] count       The number of elements.
        /// @param[in] type        The type of the values. Must be UNSIGNED_BYTE, UNSIGNED_SHORT or UNSIGNED_INT.
        /// @param[in] offset      The offset into the buffer.
        /// @return Shared pointer to newly created IndexAccessor.
        static IndexAccessorRef create(IndexBufferRef indexBuffer, GLsizei count, GLenum type, GLintptr offset);

        void bind();

        GLsizei getCount() const;
        GLenum getType() const;
        GLintptr getOffset() const;

    private:
        IndexBufferRef _buffer;
        GLsizei _count;
        GLenum _type;
        GLintptr _offset;
    };
}
