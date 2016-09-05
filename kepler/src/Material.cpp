#include "stdafx.h"
#include "Material.hpp"
#include "Technique.hpp"

namespace kepler {

    Material::Material() {
    }

    Material::~Material() noexcept {
    }

    MaterialRef Material::create() {
        return MAKE_SHARED(Material);
    }

    MaterialRef Material::create(TechniqueRef technique) {
        auto mat = create();
        mat->setTechnique(technique);
        return mat;
    }

    TechniqueRef Material::getTechnique() const {
        return _technique;
    }

    const std::string& Material::getName() const {
        return _name;
    }

    void Material::setName(const std::string& name) {
        _name.assign(name);
    }

    void Material::setTechnique(const TechniqueRef& technique) {
        if (_technique != technique) {
            if (_technique) {
                _technique->setMaterial(nullptr);
            }
            _technique = technique;
            if (_technique) {
                _technique->setMaterial(shared_from_this());
            }
        }
    }

    void Material::addParam(const MaterialParameterRef& param) {
        _parameters[param->getName()] = param;
    }

    MaterialParameterRef Material::getParam(const std::string& name) const {
        auto p = _parameters.find(name);
        if (p != _parameters.end()) {
            return p->second;
        }
        return nullptr;
    }

    void Material::bind() {
        if (_technique) {
            _technique->bind();
        }
    }
}
