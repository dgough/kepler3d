#include "stdafx.h"
#include "MaterialParameter.hpp"

namespace kepler {

    MaterialParameter::MaterialParameter(const std::string& name)
        : _name(name), _semantic(Semantic::NONE), _uniform(nullptr) {
    }

    MaterialParameter::~MaterialParameter() noexcept {
    }

    MaterialParameterRef MaterialParameter::create(const std::string& name) {
        return MAKE_SHARED(MaterialParameter, name);
    }

    const std::string& MaterialParameter::getName() const {
        return _name;
    }

    void MaterialParameter::setValue(float value) {
        _function = [value](Effect& effect, const Uniform* uniform) {
            effect.setValue(uniform, value);
        };
    }

    void MaterialParameter::setValue(int value) {
        _function = [value](Effect& effect, const Uniform* uniform) {
            effect.setValue(uniform, value);
        };
    }

    void MaterialParameter::setValue(const glm::mat4& value) {
        _function = [value](Effect& effect, const Uniform* uniform) {
            effect.setValue(uniform, value);
        };
    }

    void MaterialParameter::setValue(const glm::vec2& value) {
        _function = [value](Effect& effect, const Uniform* uniform) {
            effect.setValue(uniform, value);
        };
    }

    void MaterialParameter::setValue(const glm::vec3& value) {
        _function = [value](Effect& effect, const Uniform* uniform) {
            effect.setValue(uniform, value);
        };
    }

    void MaterialParameter::setValue(const glm::vec4& value) {
        _function = [value](Effect& effect, const Uniform* uniform) {
            effect.setValue(uniform, value);
        };
    }

    void MaterialParameter::setValue(const FunctionBinding& func) {
        _function = func;
    }

    void MaterialParameter::setValue(TextureRef texture) {
        _function = [texture](Effect& effect, const Uniform* uniform) {
            effect.setTexture(uniform, texture);
        };
    }

    MaterialParameter::Semantic MaterialParameter::getSemantic() const {
        return _semantic;
    }

    void MaterialParameter::setSemantic(Semantic semantic) {
        _semantic = semantic;
    }

    Uniform* MaterialParameter::getUniform() const {
        return _uniform;
    }

    void MaterialParameter::setUniform(Uniform* uniform) {
        _uniform = uniform;
    }

    void MaterialParameter::bind(EffectRef effect) {
        if (effect == nullptr || _uniform == nullptr) {
            return; // TODO assert?
        }
        _function(*effect, _uniform);
    }
}
