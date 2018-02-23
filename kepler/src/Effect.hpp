#pragma once

#include <map>

#include "Base.hpp"
#include "BaseGL.hpp"
#include "BaseMath.hpp"

namespace kepler {

class Uniform;

/// An effect represents a shader program.
class Effect : public std::enable_shared_from_this<Effect> {
public:
    // Use one of the static create methods.
    explicit Effect(ProgramHandle program);
    virtual ~Effect() noexcept;
    Effect(const Effect&) = delete;
    Effect& operator=(const Effect&) = delete;

    static ref<Effect> createFromFile(const char* vertexShaderPath, const char* fragmentShaderPath, const char* defines[] = nullptr, size_t defineCount = 0);
    static ref<Effect> createFromSource(const std::string& vertSource, const std::string& fragSource, const char* defines[] = nullptr, size_t defineCount = 0);

    void bind() const noexcept;
    void unbind() const noexcept;

    GLint attribLocation(const std::string& attribName) const;

    /// Returns the uniform location by searching through the pre-fetched uniforms.
    /// @param[in] uniformName The name of the uniform.
    /// @return The uniform location or -1 if not found.
    GLint findUniformLocation(const std::string& uniformName);
    /// Returns the uniform location using the gl function.
    GLint getUniformLocation(const char* uniformName) const noexcept;

    Uniform* uniform(const std::string& uniformName) const;
    ProgramHandle program() const noexcept;

    void setValue(GLint location, float value) const noexcept;
    void setValue(const Uniform* uniform, float value) const noexcept;
    void setValue(GLint location, int value) const noexcept;
    void setValue(const Uniform* uniform, int value) const noexcept;
    void setValue(const Uniform* uniform, const glm::mat3& value) const noexcept;
    void setValue(GLint location, const glm::mat4& value) const noexcept;
    void setValue(const Uniform* uniform, const glm::mat4& value) const noexcept;
    void setValue(const Uniform* uniform, const glm::vec2& value) const noexcept;
    void setValue(GLint location, const glm::vec3& value) const noexcept;
    void setValue(const Uniform* uniform, const glm::vec3& value) const noexcept;
    void setValue(const Uniform* uniform, const glm::vec4& value) const noexcept;

    void setTexture(const Uniform* uniform, ref<Texture> texture) const noexcept;

private:

    void saveAttribLocation(const GLchar* attribName, GLint location);
    void queryAttributes();
    void queryUniforms();

private:
    ProgramHandle _program;
    std::map<std::string, GLint> _attribLocations;
    std::map<std::string, std::unique_ptr<Uniform>> _uniforms;
};

/// Shader Uniform.
class Uniform {
    friend class Effect;
public:
    ~Uniform() noexcept;

    ref<Effect> effect() const;

    Uniform(const Uniform&) = delete;
    Uniform& operator=(const Uniform&) = delete;
private:
    Uniform(const std::string& name, GLint location, GLenum type);
private:
    std::string _name;
    GLint _location;
    GLenum _type;
    unsigned int _index;
    std::weak_ptr<Effect> _effect;
};
}
