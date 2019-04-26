#include "stdafx.h"
#include "Effect.hpp"
#include "Shader.hpp"
#include "Sampler.hpp"
#include "FileSystem.hpp"
#include "StringUtils.hpp"
#include "Logging.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace kepler {
namespace gl {

static constexpr const char* VERSION_STR = "#version 120\n"; // TODO move this to be platform specific.
static constexpr const char* DEFINE = "#define ";

//static GLuint loadShaderFromFile(const char* path, GLenum shaderType);
static Shader loadShaderFromSource(const std::string& source, GLenum shaderType, const char* defines[] = nullptr, size_t defineCount = 0);
static Program createAndLinkProgram(const Shader& vertShader, const Shader& fragShader);
//static GLuint loadShaderProgramFromFile(const char* vertShaderPath, const char* fragShaderPath);

Effect::Effect(Program&& program) : _program(std::move(program)) {
}

Effect::~Effect() noexcept {
    if (_program) {
        _program.destroy();
    }
}

shared_ptr<Effect> Effect::createFromFile(const char* vertexShaderPath, const char* fragmentShaderPath, const char* defines[], size_t defineCount) {
    if (vertexShaderPath == nullptr || fragmentShaderPath == nullptr) {
        return nullptr;
    }
    std::string vertSrc;
    std::string fragSrc;
    try {
        readTextFile(vertexShaderPath, vertSrc);
        readTextFile(fragmentShaderPath, fragSrc);
    }
    catch (const std::ios::failure&) {
        return nullptr;
    }
    return createFromSource(vertSrc, fragSrc, defines, defineCount);
}

void Effect::bind() const noexcept {
    glUseProgram(_program);
}

void Effect::unbind() const noexcept {
    glUseProgram(0);
}

GLint Effect::attribLocation(const std::string& attribName) const {
    auto it = _attribLocations.find(attribName);
    if (it != _attribLocations.end()) {
        return it->second;
    }
    return -1;
}

GLint Effect::findUniformLocation(const std::string& uniformName) {
    auto it = _uniforms.find(uniformName);
    if (it != _uniforms.end()) {
        return it->second->_location;
    }
    return -1;
}

GLint Effect::getUniformLocation(const char* uniformName) const noexcept {
    return _program.getUniformLocation(uniformName);
}

Uniform* Effect::uniform(const std::string& uniformName) const {
    auto it = _uniforms.find(uniformName);
    if (it != _uniforms.end()) {
        return it->second.get();
    }
    return nullptr;
}

void Effect::setValue(GLint location, float value) const noexcept {
    glUniform1f(location, value);
}

void Effect::setValue(const Uniform* uniform, float value) const noexcept {
    glUniform1f(uniform->_location, value);
}

void Effect::setValue(GLint location, int value) const noexcept {
    glUniform1i(location, value);
}

void Effect::setValue(const Uniform* uniform, int value) const noexcept {
    glUniform1i(uniform->_location, value);
}

void Effect::setValue(const Uniform* uniform, const mat3& value) const noexcept {
    glUniformMatrix3fv(uniform->_location, 1, GL_FALSE, glm::value_ptr(value));
}

void Effect::setValue(GLint location, const mat4& value) const noexcept {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Effect::setValue(const Uniform* uniform, const mat4& value) const noexcept {
    glUniformMatrix4fv(uniform->_location, 1, GL_FALSE, glm::value_ptr(value));
}

void Effect::setValue(const Uniform* uniform, const vec2& value) const noexcept {
    glUniform2f(uniform->_location, value.x, value.y);
}

void Effect::setValue(GLint location, const vec3& value) const noexcept {
    glUniform3f(location, value.x, value.y, value.z);
}

void Effect::setValue(const Uniform* uniform, const vec3& value) const noexcept {
    glUniform3f(uniform->_location, value.x, value.y, value.z);
}

void Effect::setValue(const Uniform* uniform, const vec4& value) const noexcept {
    glUniform4f(uniform->_location, value.x, value.y, value.z, value.w);
}

void Effect::setTexture(const Uniform* uniform, const shared_ptr<Texture>& texture) const noexcept {
    GLenum textureUnit = GL_TEXTURE0 + uniform->_index;
    glActiveTexture(textureUnit);
    texture->bind(uniform->_index);
    glUniform1i(uniform->_location, (GLint)uniform->_index);
}

shared_ptr<Effect> Effect::createFromSource(const std::string& vertSource, const std::string& fragSource, const char* defines[], size_t defineCount) {
    Shader vertShader = loadShaderFromSource(vertSource, GL_VERTEX_SHADER, defines, defineCount);
    Shader fragShader = loadShaderFromSource(fragSource, GL_FRAGMENT_SHADER, defines, defineCount);
    Program program = createAndLinkProgram(vertShader, fragShader);
    shared_ptr<Effect> effect = std::make_shared<Effect>(std::move(program));
    effect->queryAttributes();
    effect->queryUniforms();
    return effect;
}

void Effect::saveAttribLocation(const GLchar* attribName, GLint location) {
    _attribLocations[std::string(attribName)] = location;
}

void Effect::queryAttributes() {
    const auto activeAttributes = _program.getInt(GL_ACTIVE_ATTRIBUTES);
    const auto maxLength = _program.getInt(GL_ACTIVE_ATTRIBUTE_MAX_LENGTH);
    if (activeAttributes > 0 && maxLength > 0) {
        std::vector<GLchar> attribName(static_cast<size_t>(maxLength) + 1);
        GLint attribSize;
        GLenum attribType;
        GLint attribLocation;
        for (int i = 0; i < activeAttributes; ++i) {
            // Query attribute info.
            glGetActiveAttrib(_program, i, maxLength, nullptr, &attribSize, &attribType, attribName.data());

            // Query the pre-assigned attribute location.
            attribLocation = glGetAttribLocation(_program, attribName.data());

            // Assign the vertex attribute mapping for the effect.
            saveAttribLocation(attribName.data(), attribLocation);
        }
    }
}

void Effect::queryUniforms() {
    // Query and store uniforms from the program.
    const auto activeUniforms = _program.getInt(GL_ACTIVE_UNIFORMS);
    const auto maxLength = _program.getInt(GL_ACTIVE_UNIFORM_MAX_LENGTH);
    if (activeUniforms > 0 && maxLength > 0) {
        std::vector<GLchar> uniformName(static_cast<size_t>(maxLength) + 1);
        GLint uniformSize;
        GLenum uniformType;
        GLint uniformLocation;
        unsigned int samplerIndex = 0;
        for (int i = 0; i < activeUniforms; ++i) {
            // Query uniform info.
            glGetActiveUniform(_program, i, maxLength, nullptr, &uniformSize, &uniformType, uniformName.data());
            if (maxLength > 3) {
                // If this is an array uniform, strip array indexers off it since GL does not
                // seem to be consistent across different drivers/implementations in how it returns
                // array uniforms. On some systems it will return "u_matrixArray", while on others
                // it will return "u_matrixArray[0]".
                char* c = strrchr(uniformName.data(), '[');
                if (c) {
                    *c = '\0';
                }
            }

            // Query the pre-assigned uniform location.
            uniformLocation = glGetUniformLocation(_program, uniformName.data());

            auto uniform = std::make_unique<Uniform>(uniformName.data(), uniformLocation, uniformType, shared_from_this());
            if (uniformType == GL_SAMPLER_2D || uniformType == GL_SAMPLER_CUBE) {
                uniform->_index = samplerIndex;
                samplerIndex += uniformSize;
            }
            _uniforms[uniform->name()] = std::move(uniform);
        }
    }
}

// Uniform

Uniform::Uniform(const char* name, GLint location, GLenum type, const shared_ptr<Effect>& effect)
    : _name(name), _location(location), _type(type), _index(0), _effect(effect) {
}

Uniform::~Uniform() noexcept = default;

shared_ptr<Effect> Uniform::effect() const {
    return _effect.lock();
}

//////////////
// functions

Shader loadShaderFromSource(const std::string& source, GLenum shaderType, const char* defines[], size_t defineCount) {
    Shader shader(shaderType);
    if (!shader) {
        throw std::runtime_error("SHADER::CREATE");
    }
    std::string version;
    std::vector<const GLchar*> srcLines;
    srcLines.reserve(defineCount * 3 + 2);
    const char* sourceStr = source.c_str();

    size_t versionStart = source.find("#version");
    if (versionStart != std::string::npos) {
        if (defineCount > 0) {
            size_t versionEnd = source.find('\n', versionStart);
            if (versionEnd != std::string::npos) {
                // copy the version, including the newline
                version.assign(sourceStr + versionStart, versionEnd - versionStart + 1);
                // move the source pointer past the version line
                sourceStr += versionEnd + 1;
                srcLines.push_back(version.c_str());
            }
        }
    }
    else {
        // If the source doesn't specify a version then add our own.
        // This is used when loading glTF 1.0 shaders
        srcLines.push_back(VERSION_STR);
    }
    for (size_t i = 0; i < defineCount; ++i) {
        srcLines.push_back(DEFINE);
        srcLines.push_back(defines[i]);
        srcLines.push_back("\n");
    }
    srcLines.push_back(sourceStr);
    shader.loadSource(srcLines);
    if (!shader.compile()) {
        loge("ERROR::SHADER::COMPILE\n", shader.getInfoLog().c_str());
        throw std::runtime_error("ERROR::SHADER::COMPILE");
    }
    return shader;
}

Program createAndLinkProgram(const Shader& vertShader, const Shader& fragShader) {
    Program program(glCreateProgram());
    if (!program) {
        throw std::runtime_error("PROGRAM::CREATE");
    }
    if (!program.link(vertShader, fragShader)) {
        loge("PROGRAM::LINK\n", program.getInfoLog().c_str());
        throw std::runtime_error("PROGRAM::CREATE");
    }
    return program;
}

} // namespace gl
} // namespace kepler
