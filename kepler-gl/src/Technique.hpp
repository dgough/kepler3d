#pragma once

#include <BaseGL.hpp>
#include "AttributeSemantic.hpp"
#include "RenderState.hpp"

namespace kepler {
namespace gl {

using AttributeMap = std::map<std::string, AttributeSemantic>;

/// A technique describes the shading used for a material.
class Technique {
public:
    /// Use Technique::create() instead.
    Technique();
    virtual ~Technique() noexcept;
    static shared_ptr<Technique> create();
    static shared_ptr<Technique> create(const shared_ptr<Effect>& effect);

    shared_ptr<Effect> effect() const;
    const AttributeMap& attributes() const;
    const std::map<std::string, std::string>& uniforms() const;

    const std::map<std::string, shared_ptr<MaterialParameter>>& semantics() const;

    /// Returns the RenderState for this technique.
    RenderState& renderState();

    void setAttribute(const std::string& glslName, AttributeSemantic semantic);

    /// Sets the name of the parameter that is used to get the value for the given shader uniform.
    /// Material parameters can be defined in either the technique or the parent material. This method tells the technique which
    /// material parameter it should look for. If it is not set in the technique, it will look in the material.
    /// @param[in] glslName  The name of the shader uniform.
    /// @param[in] paramName The name of the material parameter.
    void setUniformName(const std::string& glslName, const std::string& paramName);

    void setUniform(const std::string& glslName, const shared_ptr<MaterialParameter>& param);
    void setSemanticUniform(const std::string& glslName, MaterialParameter::Semantic semantic);
    void setEffect(const shared_ptr<Effect>& effect);
    void setMaterial(const shared_ptr<Material>& material);

    void bind(); // TODO remove and use a pass?
    void findValues(std::vector<shared_ptr<MaterialParameter>>& values);

    /// Finds the material parameter with the given name.
    shared_ptr<MaterialParameter> findValueParameter(const std::string& paramName);

private:
    Technique(const Technique&) = delete;
    Technique& operator=(const Technique&) = delete;

    bool updateUniform(MaterialParameter& materialParam, const std::string& uniformName);
private:
    std::string _name;
    shared_ptr<Effect> _effect;

    // attributes: <shaderAttribName, paramName>
    AttributeMap _attributes;

    // uniforms:   <shaderAttribName, paramName>
    std::map<std::string, std::string> _uniforms;

    // key = parameterName
    std::map<std::string, shared_ptr<MaterialParameter>> _semantics;
    // key = parameterName
    std::map<std::string, shared_ptr<MaterialParameter>> _values;

    std::weak_ptr<Material> _material;

    // states
    RenderState _renderState;
};
}
}
