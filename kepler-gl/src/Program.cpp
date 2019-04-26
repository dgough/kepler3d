#include "stdafx.h"
#include "Program.hpp"

namespace kepler {
namespace gl {

static_assert(sizeof(Program) == sizeof(ProgramHandle), "Ensure no vtable created");

Program::~Program() noexcept {
    if (_handle) {
        glDeleteShader(_handle);
    }
}

Program::Program(Program&& other) noexcept : _handle(other._handle) {
    other._handle = 0;
}

Program& Program::operator=(Program&& other) noexcept {
    if (this != &other) {
        std::swap(_handle, other._handle);
        other.destroy();
    }
    return *this;
}

void Program::attach(const Shader& shader) const {
    glAttachShader(_handle, shader);
}

bool Program::link() const {
    glLinkProgram(_handle);
    return getInt(GL_LINK_STATUS) != 0;
}

bool Program::link(const Shader& vert, const Shader& frag) const {
    glAttachShader(_handle, vert);
    glAttachShader(_handle, frag);
    return link();
}

void Program::destroy() {
    if (_handle) {
        glDeleteProgram(_handle);
        _handle = 0;
    }
}

GLint Program::getInt(GLenum pname) const {
    GLint value;
    glGetProgramiv(_handle, pname, &value);
    return value;
}

std::string Program::getInfoLog() const {
    const auto length = getInfoLogLength();
    std::string text;
    text.resize(length);
    GLsizei newLength = 0;
    glGetProgramInfoLog(_handle, length, &newLength, const_cast<char*>(text.data()));
    text.resize(newLength);
    return text;
}

GLint Program::getInfoLogLength() const {
    return getInt(GL_INFO_LOG_LENGTH);
}

GLint Program::getUniformLocation(const char* uniformName) const {
    return glGetUniformLocation(_handle, uniformName);
}

} // namespace gl
} // namespace kepler
