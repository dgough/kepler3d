#include "stdafx.h"
#include "Shader.hpp"

namespace kepler {
namespace gl {

static_assert(sizeof(Shader) == sizeof(ShaderHandle), "Ensure no vtable created");

Shader::Shader(GLenum shaderType) : _handle(glCreateShader(shaderType)) {
}

Shader::~Shader() {
    if (_handle) {
        glDeleteShader(_handle);
    }
}

Shader::Shader(Shader&& other) noexcept : _handle(other._handle) {
    other._handle = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        std::swap(_handle, other._handle);
        other.destroy();
    }
    return *this;
}

void Shader::loadSource(const std::vector<const char*>& source) const {
    glShaderSource(_handle, static_cast<GLsizei>(source.size()), source.data(), nullptr);
}

bool Shader::compile() const {
    if (_handle) {
        GLint success = GL_FALSE;
        glCompileShader(_handle);
        glGetShaderiv(_handle, GL_COMPILE_STATUS, &success);
        return success == GL_TRUE;
    }
    return false;
}

void Shader::destroy() {
    if (_handle) {
        glDeleteShader(_handle);
        _handle = 0;
    }
}

GLenum Shader::getType() const {
    GLint type = 0;
    glGetShaderiv(_handle, GL_SHADER_TYPE, &type);
    return static_cast<GLenum>(type);
}

std::string Shader::getInfoLog() const {
    const auto length = getInfoLogLength();
    std::string text;
    text.resize(length);
    GLsizei newLength = 0;
    glGetShaderInfoLog(_handle, length, &newLength, const_cast<char*>(text.data()));
    text.resize(newLength);
    return text;
}

GLint Shader::getInfoLogLength() const {
    GLint length = 0;
    glGetShaderiv(_handle, GL_INFO_LOG_LENGTH, &length);
    return length;
}

} // namespace gl
} // namespace kepler
