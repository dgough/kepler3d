#include "stdafx.h"
#include "Mesh.hpp"
#include "MeshPrimitive.hpp"

namespace kepler {
namespace gl {

Mesh::Mesh() {
}

Mesh::Mesh(const shared_ptr<MeshPrimitive>& primitive) : _primitives{primitive} {
}

Mesh::~Mesh() noexcept {
}

shared_ptr<Mesh> Mesh::create() {
    return std::make_shared<Mesh>();
}

shared_ptr<Mesh> Mesh::create(const shared_ptr<MeshPrimitive>& primitive) {
    return std::make_shared<Mesh>(primitive);
}

void Mesh::addMeshPrimitive(const shared_ptr<MeshPrimitive>& primitive) {
    if (_box.empty()) {
        _box = primitive->boundingBox();
    }
    else {
        _box.merge(primitive->boundingBox());
    }
    _primitives.push_back(std::move(primitive));
}

void Mesh::setName(const char* name) {
    //if (name != nullptr && *name != '\0') {
    //    _name = std::make_unique<std::string>(name);
    //}
}

void Mesh::setName(const std::string& name) {
    //_name = std::make_unique<std::string>(name);
}

shared_ptr<MeshPrimitive> Mesh::primitiveAt(size_t index) const {
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

void Mesh::setNode(const shared_ptr<Node>& node) {
    for (const auto& primitive : _primitives) {
        primitive->setNode(node);
    }
}
}
}
