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

    EffectRef Technique::effect() const {
        return _effect;
    }

    const AttributeMap& Technique::attributes() const {
        return _attributes;
    }

    const std::map<std::string, std::string>& Technique::uniforms() const {
        return _uniforms;
    }

    const std::map<std::string, MaterialParameterRef>& Technique::semantics() const {
        return _semantics;
    }

    RenderState& Technique::renderState() {
        return _renderState;
    }

    void Technique::setAttribute(const std::string& glslName, AttributeSemantic semantic) {
        _attributes[glslName] = semantic;
    }

    void Technique::setUniformName(const std::string& glslName, const std::string& paramName) {
        _uniforms[glslName] = paramName;
    }

    void Technique::setUniform(const std::string& glslName, MaterialParameterRef param) {
        setUniformName(glslName, param->name());
        _values[param->name()] = param;
        updateUniform(*param, glslName);
    }

    void Technique::setSemanticUniform(const std::string& glslName, const std::string& paramName, MaterialParameter::Semantic semantic) {
        auto p = MaterialParameter::create(paramName);
        p->setSemantic(semantic);
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
                    auto param = material->param(paramName);
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
        if (_effect && (materialParam.uniform() == nullptr || materialParam.uniform()->effect() != _effect)) {
            Uniform* uniform = _effect->uniform(uniformName);
            if (uniform == nullptr) {
                return false;
            }
            materialParam.setUniform(uniform);
        }
        return true;
    }
}
