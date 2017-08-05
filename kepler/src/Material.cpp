#include "stdafx.h"
#include "Material.hpp"
#include "Technique.hpp"

namespace kepler {

    ref<Material> Material::create() {
        return std::make_shared<Material>();
    }

    ref<Material> Material::create(ref<Technique> technique) {
        auto mat = create();
        mat->setTechnique(technique);
        return mat;
    }

    ref<Technique> Material::technique() const {
        return _technique;
    }

    const std::string& Material::name() const {
        return _name;
    }

    void Material::setName(const std::string& name) {
        _name.assign(name);
    }

    void Material::setTechnique(const ref<Technique>& technique) {
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

    void Material::addParam(const ref<MaterialParameter>& param) {
        _parameters[param->name()] = param;
    }

    ref<MaterialParameter> Material::param(const std::string& name) const {
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
