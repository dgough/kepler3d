#pragma once

#include <BaseGL.hpp>
#include <OpenGL.hpp>

namespace kepler {
namespace gl {

// Generic OpenGL buffer.
// Target is the target parameter from glBindBuffer.
template<GLenum Target>
class Buffer {
public:
    Buffer() : _handle(0) {}

    Buffer(GLsizeiptr size, const GLvoid* data, GLenum usage) {
        glGenBuffers(1, &_handle);
        glBindBuffer(Target, _handle);
        glBufferData(Target, size, data, usage);
    }

    ~Buffer() noexcept {
        if (_handle) {
            glDeleteBuffers(1, &_handle);
        }
    }

    // Not copyable
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    // Movable
    Buffer(Buffer&& other) : _handle(other._handle) {
        other._handle = 0;
    }

    Buffer& operator=(Buffer&& other) {
        if (this != &other) {
            std::swap(_handle, other._handle);
            other.destroy();
        }
        return *this;
    }

    /// Binds this buffer.
    void bind() const {
        glBindBuffer(Target, _handle);
    }

    void destroy() {
        if (_handle) {
            glDeleteBuffers(1, &_handle);
            _handle = 0;
        }
    }

    // Allows Buffer to be used in an if statement
    explicit operator bool() const noexcept {
        return _handle != 0;
    }

private:
    BufferHandle _handle;
};

} // namespace gl
} // namespace kepler
