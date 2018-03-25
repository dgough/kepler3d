#include "stdafx.h"
#include "MaterialParameter.hpp"

namespace kepler {
namespace gl {

MaterialParameter::MaterialParameter(const std::string& name)
    : _name(name), _semantic(Semantic::NONE), _uniform(nullptr) {
}

MaterialParameter::MaterialParameter(std::string&& name)
    : _name(std::move(name)), _semantic(Semantic::NONE), _uniform(nullptr) {
}

ref<MaterialParameter> MaterialParameter::create(const std::string& name) {
    return std::make_shared<MaterialParameter>(name);
}

ref<MaterialParameter> MaterialParameter::create(std::string&& name) {
    return std::make_shared<MaterialParameter>(std::move(name));
}

const std::string& MaterialParameter::name() const {
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

void MaterialParameter::setValue(const ref<Texture>& texture) {
    _function = [texture](Effect& effect, const Uniform* uniform) {
        effect.setTexture(uniform, texture);
    };
}

MaterialParameter::Semantic MaterialParameter::semantic() const {
    return _semantic;
}

void MaterialParameter::setSemantic(Semantic semantic) {
    _semantic = semantic;
}

Uniform* MaterialParameter::uniform() const {
    return _uniform;
}

void MaterialParameter::setUniform(Uniform* uniform) {
    _uniform = uniform;
}

void MaterialParameter::bind(Effect& effect) const {
    if (_uniform == nullptr) {
        return; // TODO assert?
    }
    _function(effect, _uniform);
}
}
}
