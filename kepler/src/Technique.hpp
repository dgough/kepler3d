#pragma once

#include "Base.hpp"
#include "AttributeSemantic.hpp"
#include "RenderState.hpp"

namespace kepler {

using AttributeMap = std::map<std::string, AttributeSemantic>;

/// A technique describes the shading used for a material.
class Technique {
public:
    /// Use Technique::create() instead.
    Technique();
    virtual ~Technique() noexcept;
    static ref<Technique> create();
    static ref<Technique> create(ref<Effect> effect);

    ref<Effect> effect() const;
    const AttributeMap& attributes() const;
    const std::map<std::string, std::string>& uniforms() const;

    const std::map<std::string, ref<MaterialParameter>>& semantics() const;

    /// Returns the RenderState for this technique.
    RenderState& renderState();

    void setAttribute(const std::string& glslName, AttributeSemantic semantic);

    /// Sets the name of the parameter that is used to get the value for the given shader uniform.
    /// Material parameters can be defined in either the technique or the parent material. This method tells the technique which
    /// material parameter it should look for. If it is not set in the technique, it will look in the material.
    /// @param[in] glslName  The name of the shader uniform.
    /// @param[in] paramName The name of the material parameter.
    void setUniformName(const std::string& glslName, const std::string& paramName);

    void setUniform(const std::string& glslName, ref<MaterialParameter> param);
    void setSemanticUniform(const std::string& glslName, MaterialParameter::Semantic semantic);
    void setEffect(ref<Effect> effect);
    void setMaterial(ref<Material> material);

    void bind(); // TODO remove and use a pass?
    void findValues(std::vector<ref<MaterialParameter>>& values);

    /// Finds the material parameter with the given name.
    ref<MaterialParameter> findValueParameter(const std::string& paramName);

private:
    Technique(const Technique&) = delete;
    Technique& operator=(const Technique&) = delete;

    bool updateUniform(MaterialParameter& materialParam, const std::string& uniformName);
private:
    std::string _name;
    ref<Effect> _effect;

    // attributes: <shaderAttribName, paramName>
    AttributeMap _attributes;

    // uniforms:   <shaderAttribName, paramName>
    std::map<std::string, std::string> _uniforms;

    // key = parameterName
    std::map<std::string, ref<MaterialParameter>> _semantics;
    // key = parameterName
    std::map<std::string, ref<MaterialParameter>> _values;

    std::weak_ptr<Material> _material;

    // states
    RenderState _renderState;
};
}
