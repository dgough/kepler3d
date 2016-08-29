#pragma once

#include "Base.hpp"
#include "Attribute.hpp"
#include "RenderState.hpp"

namespace kepler {

    using AttirbuteMap = std::map<std::string, Attribute>;

    /// A technique describes the shading used for a material.
    class Technique {
        ALLOW_MAKE_SHARED(Technique);
    public:
        virtual ~Technique() noexcept;
        static TechniqueRef create();

        EffectRef getEffect() const;
        const AttirbuteMap& getAttirbutes() const;
        const std::map<std::string, std::string>& getUniforms() const;

        const std::map<std::string, MaterialParameterRef>& getSemantics() const;

        /// Returns the RenderState for this technique.
        RenderState& getRenderState();

        void setAttribute(const std::string& glslName, const std::string& paramName, Attribute::Semantic semantic, MaterialParameter::Type type);
        void setUniform(const std::string& glslName, const std::string& paramName);
        void setUniformValue(const std::string& glslName, MaterialParameterRef param);
        void setSemanticUniform(const std::string& glslName, const std::string& paramName, MaterialParameter::Semantic semantic, MaterialParameter::Type type);
        void setEffect(EffectRef effect);
        void setMaterial(MaterialRef material);

        void bind(); // TODO remove and use a pass?
        void findValues(std::vector<MaterialParameterRef>& values);

        /// Finds the material parameter with the given name.
        MaterialParameterRef findValueParameter(const std::string& paramName);

    private:
        Technique();
        Technique(const Technique&) = delete;
        Technique& operator=(const Technique&) = delete;

        bool updateUniform(MaterialParameter& materialParam, const std::string& uniformName);
    private:
        std::string _name;
        EffectRef _effect;

        // attributes: <shaderAttribName, paramName>
        AttirbuteMap _attributes;

        // uniforms:   <shaderAttribName, paramName>
        std::map<std::string, std::string> _uniforms;

        // key = parameterName
        std::map<std::string, MaterialParameterRef> _semantics;
        // key = parameterName
        std::map<std::string, MaterialParameterRef> _values;

        std::weak_ptr<Material> _material;

        // states
        RenderState _renderState;

        // Should this go in a pass?
        //VertexAttributeBindingRef _vaBinding;
    };
}
