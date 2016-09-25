#include "stdafx.h"
#include "Effect.hpp"
#include "BaseGL.hpp"
#include "Sampler.hpp"
#include "FileSystem.hpp"
#include "StringUtils.hpp"
#include "Logging.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace kepler {

    static const std::string VERSION_STR("#version 120\n"); // TODO move this to be platform specific.

    //static GLuint loadShaderFromFile(const char* path, GLenum shaderType);
    static GLuint loadShaderFromSource(const std::string& source, GLenum shaderType);
    static GLuint createAndLinkProgram(GLuint vertShader, GLuint fragShader);
    //static GLuint loadShaderProgramFromFile(const char* vertShaderPath, const char* fragShaderPath);

    Effect::Effect(ProgramHandle program) : _program(program) {
    }

    Effect::~Effect() noexcept {
        if (_program != 0) {
            glDeleteProgram(_program);
        }
    }

    EffectRef Effect::createFromFile(const char* vertexShaderPath, const char* fragmentShaderPath) {
        if (vertexShaderPath == nullptr || fragmentShaderPath == nullptr) {
            return nullptr;
        }
        std::string vertSrc;
        std::string fragSrc;
        if (!readTextFile(vertexShaderPath, vertSrc)) {
            return nullptr;
        }
        if (!readTextFile(fragmentShaderPath, fragSrc)) {
            return nullptr;
        }
        return createFromSource(vertSrc, fragSrc);
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
        return glGetUniformLocation(_program, uniformName);
    }

    Uniform* Effect::uniform(const std::string& uniformName) const {
        auto it = _uniforms.find(uniformName);
        if (it != _uniforms.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    ProgramHandle Effect::program() const noexcept {
        return _program;
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

    void Effect::setValue(const Uniform* uniform, const glm::mat3& value) const noexcept {
        glUniformMatrix3fv(uniform->_location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Effect::setValue(GLint location, const glm::mat4& value) const noexcept {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Effect::setValue(const Uniform* uniform, const glm::mat4& value) const noexcept {
        glUniformMatrix4fv(uniform->_location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Effect::setValue(const Uniform* uniform, const glm::vec2& value) const noexcept {
        glUniform2f(uniform->_location, value.x, value.y);
    }

    void Effect::setValue(GLint location, const glm::vec3 & value) const noexcept {
        glUniform3f(location, value.x, value.y, value.z);
    }

    void Effect::setValue(const Uniform* uniform, const glm::vec3& value) const noexcept {
        glUniform3f(uniform->_location, value.x, value.y, value.z);
    }

    void Effect::setValue(const Uniform* uniform, const glm::vec4& value) const noexcept {
        glUniform4f(uniform->_location, value.x, value.y, value.z, value.w);
    }

    void Effect::setTexture(const Uniform* uniform, TextureRef texture) const noexcept {
        GLenum textureUnit = GL_TEXTURE0 + uniform->_index;
        glActiveTexture(textureUnit);
        texture->bind(uniform->_index);
        glUniform1i(uniform->_location, (GLint)uniform->_index);
    }

    EffectRef Effect::createFromSource(const std::string& vertSource, const std::string& fragSource) {
        GLuint vertShader = loadShaderFromSource(vertSource, GL_VERTEX_SHADER);
        GLuint fragShader = loadShaderFromSource(fragSource, GL_FRAGMENT_SHADER);
        ProgramHandle program = createAndLinkProgram(vertShader, fragShader);
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
        if (program == 0) {
            return nullptr;
        }
        EffectRef effect = MAKE_SHARED(Effect, program);
        effect->queryAttributes();
        effect->queryUniforms();
        return effect;
    }

    void Effect::saveAttribLocation(const GLchar* attribName, GLint location) {
        _attribLocations[std::string(attribName)] = location;
    }

    void Effect::queryAttributes() {
        GLint activeAttributes;
        glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &activeAttributes);
        if (activeAttributes > 0) {
            GLint length;
            glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &length);
            if (length > 0) {
                std::unique_ptr<GLchar[]> attribName(new GLchar[length + 1]);
                GLint attribSize;
                GLenum attribType;
                GLint attribLocation;
                for (int i = 0; i < activeAttributes; ++i) {
                    // Query attribute info.
                    glGetActiveAttrib(_program, i, length, nullptr, &attribSize, &attribType, &attribName[0]);
                    attribName[length] = '\0';

                    // Query the pre-assigned attribute location.
                    attribLocation = glGetAttribLocation(_program, &attribName[0]);

                    // Assign the vertex attribute mapping for the effect.
                    saveAttribLocation(&attribName[0], attribLocation);
                }
            }
        }
    }

    void Effect::queryUniforms() {
        // Query and store uniforms from the program.
        GLint activeUniforms;
        glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &activeUniforms);
        if (activeUniforms > 0) {
            GLint length;
            glGetProgramiv(_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &length);
            if (length > 0) {
                std::unique_ptr<GLchar[]> uniformName(new GLchar[length + 1]);
                GLint uniformSize;
                GLenum uniformType;
                GLint uniformLocation;
                unsigned int samplerIndex = 0;
                for (int i = 0; i < activeUniforms; ++i) {
                    // Query uniform info.
                    glGetActiveUniform(_program, i, length, nullptr, &uniformSize, &uniformType, &uniformName[0]);
                    uniformName[length] = '\0';  // null terminate
                    if (length > 3) {
                        // If this is an array uniform, strip array indexers off it since GL does not
                        // seem to be consistent across different drivers/implementations in how it returns
                        // array uniforms. On some systems it will return "u_matrixArray", while on others
                        // it will return "u_matrixArray[0]".
                        char* c = strrchr(&uniformName[0], '[');
                        if (c) {
                            *c = '\0';
                        }
                    }

                    // Query the pre-assigned uniform location.
                    uniformLocation = glGetUniformLocation(_program, &uniformName[0]);

                    std::unique_ptr<Uniform> uniform(new Uniform(std::string(&uniformName[0]), uniformLocation, uniformType));
                    uniform->_effect = shared_from_this();
                    if (uniformType == GL_SAMPLER_2D || uniformType == GL_SAMPLER_CUBE) {
                        uniform->_index = samplerIndex;
                        samplerIndex += uniformSize;
                    }
                    _uniforms[uniform->_name] = std::move(uniform);
                }
            }
        }
    }

    // Uniform

    Uniform::Uniform(const std::string& name, GLint location, GLenum type) : _name(name), _location(location), _type(type), _index(0) {

    }

    Uniform::~Uniform() noexcept {

    }

    EffectRef Uniform::effect() const {
        return _effect.lock();
    }

    //////////////
    // functions

    // Loads shader from a file.
    //GLuint loadShaderFromFile(const char* path, GLenum shaderType) {
    //    std::string source;
    //    if (!readTextFile(path, source)) {
    //        loge("ERROR::SHADER::FILE ", path);
    //        return 0;
    //    }
    //    return loadShaderFromSource(source, shaderType);
    //}

    GLuint loadShaderFromSource(const std::string& source, GLenum shaderType) {
        GLuint shaderId = glCreateShader(shaderType);
        if (shaderId == 0) {
            loge("SHADER::CREATE");
            return 0;
        }

        const GLchar* src[2];
        GLsizei count = 1;
        src[0] = source.c_str();

        // If the source doesn't specify a version then add our own.
        if (!startsWith(source, "#version")) {
            count = 2;
            src[1] = src[0];
            src[0] = VERSION_STR.c_str();
        }
        glShaderSource(shaderId, count, src, nullptr);
        glCompileShader(shaderId);

        GLint success;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (!success) {
            constexpr size_t LOG_SIZE = 512;
            GLchar infoLog[LOG_SIZE];
            glGetShaderInfoLog(shaderId, LOG_SIZE, nullptr, infoLog);
            loge("ERROR::SHADER::COMPILE\n", infoLog);
            glDeleteShader(shaderId);
            return 0;
        }
        return shaderId;
    }

    ProgramHandle createAndLinkProgram(GLuint vertShader, GLuint fragShader) {
        if (vertShader == 0) {
            loge("VERTEX_SHADER_MISSING");
            return 0;
        }
        if (fragShader == 0) {
            loge("FRAG_SHADER_MISSING");
            return 0;
        }
        ProgramHandle program = glCreateProgram();
        if (program == 0) {
            loge("PROGRAM::CREATE");
            return 0;
        }
        glAttachShader(program, vertShader);
        glAttachShader(program, fragShader);
        glLinkProgram(program);
        GLint success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            constexpr size_t LOG_SIZE = 512;
            GLchar infoLog[LOG_SIZE];
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            loge("PROGRAM::LINK\n", infoLog);
            glDeleteProgram(program);
            return 0;
        }
        return program;
    }

    //ProgramHandle loadShaderProgramFromFile(const char* vertShaderPath, const char* fragShaderPath) {
    //    GLuint vertShader = loadShaderFromFile(vertShaderPath, GL_VERTEX_SHADER);
    //    GLuint fragShader = loadShaderFromFile(fragShaderPath, GL_FRAGMENT_SHADER);
    //    ProgramHandle program = createAndLinkProgram(vertShader, fragShader);
    //    glDeleteShader(vertShader);
    //    glDeleteShader(fragShader);
    //    return program;
    //}
}
