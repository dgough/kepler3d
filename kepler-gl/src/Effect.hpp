#pragma once

#include <map>

#include <BaseGL.hpp>
#include <OpenGL.hpp>
#include <BaseMath.hpp>
#include <Program.hpp>

namespace kepler {
namespace gl {

class Uniform;

/// An effect represents a shader program.
class Effect : public std::enable_shared_from_this<Effect> {
public:
    // Use one of the static create methods.
    explicit Effect(Program&& program);
    virtual ~Effect() noexcept;
    Effect(const Effect&) = delete;
    Effect& operator=(const Effect&) = delete;

    static shared_ptr<Effect> createFromFile(const char* vertexShaderPath, const char* fragmentShaderPath, const char* defines[] = nullptr, size_t defineCount = 0);
    static shared_ptr<Effect> createFromSource(const std::string& vertSource, const std::string& fragSource, const char* defines[] = nullptr, size_t defineCount = 0);

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

    void setValue(GLint location, float value) const noexcept;
    void setValue(const Uniform* uniform, float value) const noexcept;
    void setValue(GLint location, int value) const noexcept;
    void setValue(const Uniform* uniform, int value) const noexcept;
    void setValue(const Uniform* uniform, const mat3& value) const noexcept;
    void setValue(GLint location, const mat4& value) const noexcept;
    void setValue(const Uniform* uniform, const mat4& value) const noexcept;
    void setValue(const Uniform* uniform, const vec2& value) const noexcept;
    void setValue(GLint location, const vec3& value) const noexcept;
    void setValue(const Uniform* uniform, const vec3& value) const noexcept;
    void setValue(const Uniform* uniform, const vec4& value) const noexcept;

    void setTexture(const Uniform* uniform, const shared_ptr<Texture>& texture) const noexcept;

private:

    void saveAttribLocation(const GLchar* attribName, GLint location);
    void queryAttributes();
    void queryUniforms();

private:
    Program _program;
    std::map<std::string, GLint> _attribLocations;
    std::map<std::string, std::unique_ptr<Uniform>> _uniforms;
};

/// Shader Uniform.
class Uniform final {
    friend class Effect;
public:
    Uniform(const char* name, GLint location, GLenum type, const shared_ptr<Effect>& effect);
    ~Uniform() noexcept;

    shared_ptr<Effect> effect() const;

    Uniform(const Uniform&) = delete;
    Uniform& operator=(const Uniform&) = delete;

    const std::string& name() const {
        return _name;
    }
private:
    std::string _name;
    GLint _location;
    GLenum _type;
    unsigned int _index;
    std::weak_ptr<Effect> _effect;
};

} // namespace gl
} // namespace kepler
