#include "stdafx.h"
#include "Technique.hpp"
#include "Effect.hpp"
#include "Material.hpp"

namespace kepler {

    Technique::Technique() {
    }

    Technique::~Technique() noexcept {
    }

    TechniqueRef Technique::create() {
        return MAKE_SHARED(Technique);
    }

    TechniqueRef Technique::create(EffectRef effect) {
        auto tech = create();
        tech->setEffect(effect);
        return tech;
    }

    EffectRef Technique::getEffect() const {
        return _effect;
    }

    const AttirbuteMap& Technique::getAttirbutes() const {
        return _attributes;
    }

    const std::map<std::string, std::string>& Technique::getUniforms() const {
        return _uniforms;
    }

    const std::map<std::string, MaterialParameterRef>& Technique::getSemantics() const {
        return _semantics;
    }

    RenderState& Technique::getRenderState() {
        return _renderState;
    }

    void Technique::setAttribute(const std::string& glslName, const std::string& paramName, Attribute::Semantic semantic, MaterialParameter::Type type) {
        _attributes[glslName] = Attribute(paramName, semantic, type);
    }

    void Technique::setPositionAttribute3f(const std::string& glslName) {
        setAttribute(glslName, "position", Attribute::Semantic::POSITION, MaterialParameter::Type::FLOAT_VEC3);
    }

    void Technique::setNormalAttribute3f(const std::string& glslName) {
        setAttribute(glslName, "normal", Attribute::Semantic::NORMAL, MaterialParameter::Type::FLOAT_VEC3);
    }

    void Technique::setUniformName(const std::string& glslName, const std::string& paramName) {
        _uniforms[glslName] = paramName;
    }

    void Technique::setUniform(const std::string& glslName, MaterialParameterRef param) {
        setUniformName(glslName, param->getName());
        _values[param->getName()] = param;
        updateUniform(*param, glslName);
    }

    void Technique::setSemanticUniform(const std::string& glslName, const std::string& paramName, MaterialParameter::Semantic semantic, MaterialParameter::Type type) {
        auto p = MaterialParameter::create(paramName);
        p->setSemantic(semantic, type);
        _semantics[paramName] = p;
        updateUniform(*p, glslName);
    }

    void Technique::setEffect(EffectRef effect) {
        _effect = effect;
    }

    void Technique::setMaterial(MaterialRef material) {
        _material = material;
    }

    void Technique::bind() {
        if (_effect) {
            _effect->bind();
            _renderState.bind();
        }
    }

    void Technique::findValues(std::vector<MaterialParameterRef>& values) {
        values.clear();
        if (_effect) {
            auto material = _material.lock();
            for (auto& uniformIt : _uniforms) {
                const std::string& uniformName = uniformIt.first;
                const auto& paramName = uniformIt.second;

                // TODO don't do this. This shouldn't be necessary.

                // TODO This is a mess. Needs to be improved.

                MaterialParameterRef materialParam = findValueParameter(paramName);

                if (materialParam == nullptr && material) {
                    auto param = material->getParam(paramName);
                    if (param) {
                        materialParam = param;
                    }
                }

                if (materialParam) {
                    if (updateUniform(*materialParam, uniformName)) {
                        values.push_back(materialParam);
                    }
                }
            }
        }
    }

    MaterialParameterRef Technique::findValueParameter(const std::string& paramName) {
        auto p = _values.find(paramName);
        if (p != _values.end()) {
            return p->second;
        }
        // TODO semantics and look in the parent material?
        return nullptr;
    }

    bool Technique::updateUniform(MaterialParameter& materialParam, const std::string& uniformName) {
        if (_effect && (materialParam.getUniform() == nullptr || materialParam.getUniform()->getEffect() != _effect)) {
            Uniform* uniform = _effect->getUniform(uniformName);
            if (uniform == nullptr) {
                return false;
            }
            materialParam.setUniform(uniform);
        }
        return true;
    }
}
