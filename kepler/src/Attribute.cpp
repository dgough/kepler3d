#include "stdafx.h"
#include "Attribute.hpp"

namespace kepler {

    Attribute::Attribute()
        : _semantic(Semantic::POSITION), _type(MaterialParameter::Type::FLOAT_VEC3) {
    }

    Attribute::Attribute(const std::string& name, Semantic semantic, MaterialParameter::Type type)
        : _name(name), _semantic(semantic), _type(type) {
    }

    const std::string Attribute::getName() const {
        return _name;
    }

    const Attribute::Semantic& Attribute::getSemantic() const {
        return _semantic;
    }

    const MaterialParameter::Type& Attribute::getType() const {
        return _type;
    }
}
