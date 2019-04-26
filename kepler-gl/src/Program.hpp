#pragma once

#include <BaseGL.hpp>
#include <OpenGL.hpp>
#include <Shader.hpp>

#include <vector>

namespace kepler {
namespace gl {

/// Wrapper class for the OpenGL program object.
class Program final {
public:
    Program() : _handle(0) {}
    explicit Program(ProgramHandle handle) : _handle(handle) {}

    /// Automatically calls glDeleteShader.
    ~Program() noexcept;

    // Not copyable
    Program(const Program&) = delete;
    Program& operator=(const Program&) = delete;

    // Movable
    Program(Program&& other) noexcept;
    Program& operator=(Program&& other) noexcept;

    void attach(const Shader& shader) const;

    bool link() const;
    bool link(const Shader& vert, const Shader& frag) const;

    void destroy();

    GLint getInt(GLenum pname) const;

    std::string getInfoLog() const;

    // Returns the number of characters in the information log for shader including the null termination character.
    GLint getInfoLogLength() const;

    GLint getUniformLocation(const char* uniformName) const;

    /// Returns true if this wrapper object is associated with an OpenGL shader object.
    /// Returns false if the handle is zero.
    explicit operator bool() const noexcept {
        return _handle != 0;
    }

    OPENGL_HPP_TYPESAFE_EXPLICIT operator ProgramHandle() const {
        return _handle;
    }

private:
    ProgramHandle _handle;
};

} // namespace gl
} // namespace kepler
