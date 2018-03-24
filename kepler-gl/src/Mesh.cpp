#include "stdafx.h"
#include "Mesh.hpp"
#include "MeshPrimitive.hpp"

namespace kepler {

Mesh::Mesh() {
}

Mesh::Mesh(const ref<MeshPrimitive>& primitive) : _primitives{primitive} {
}

Mesh::~Mesh() noexcept {
}

ref<Mesh> Mesh::create() {
    return std::make_shared<Mesh>();
}

ref<Mesh> Mesh::create(const ref<MeshPrimitive>& primitive) {
    return std::make_shared<Mesh>(primitive);
}

void Mesh::addMeshPrimitive(const ref<MeshPrimitive>& primitive) {
    if (_box.empty()) {
        _box = primitive->boundingBox();
    }
    else {
        _box.merge(primitive->boundingBox());
    }
    _primitives.push_back(std::move(primitive));
}

void Mesh::setName(const char* name) {
    if (name != nullptr && *name != '\0') {
        _name = std::make_unique<std::string>(name);
    }
}

void Mesh::setName(const std::string& name) {
    _name = std::make_unique<std::string>(name);
}

ref<MeshPrimitive> Mesh::primitiveAt(size_t index) const {
    return _primitives.at(index);
}

MeshPrimitive* Mesh::primitivePtr(size_t index) const {
    return _primitives[index].get();
}

size_t Mesh::primitiveCount() const {
    return _primitives.size();
}

const BoundingBox& Mesh::boundingBox() const {
    return _box;
}

void Mesh::setNode(const ref<Node>& node) {
    for (const auto& primitive : _primitives) {
        primitive->setNode(node);
    }
}
}
