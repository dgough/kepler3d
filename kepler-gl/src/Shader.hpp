#pragma once

#include <BaseGL.hpp>
#include <OpenGL.hpp>

#include <vector>

namespace kepler {
namespace gl {

/// Wrapper class for the OpenGL shader object.
class Shader final {
public:
    Shader() : _handle(0) {}
    explicit Shader(GLenum shaderType);

    /// Automatically calls glDeleteShader.
    ~Shader() noexcept;
    
    // Not copyable
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Movable
    Shader(Shader&& other);
    Shader& operator=(Shader&& other);

    void loadSource(const std::vector<const char*>& source) const;

    /// Compiles the shader. Returns true on success; false otherwise.
    bool compile() const;

    /// Attaches this shader to the given program.
    void attachToProgram(GLuint program) const {
        glAttachShader(program, _handle);
    }

    void destroy();

    /// Retruns the shader type: GL_VERTEX_SHADER, GL_FRAGMENT_SHADER or GL_GEOMETRY_SHADER
    GLenum getType() const;

    std::string getInfoLog() const;

    // Returns the number of characters in the information log for shader including the null termination character.
    GLint getInfoLogLength() const;

    /// Returns true if this wrapper object is associated with an OpenGL shader object.
    /// Returns false if the handle is zero.
    explicit operator bool() const noexcept {
        return _handle != 0;
    }

    OPENGL_HPP_TYPESAFE_EXPLICIT operator ShaderHandle() const {
        return _handle;
    }

private:
    ShaderHandle _handle;
};

} // namespace gl
} // namespace kepler
