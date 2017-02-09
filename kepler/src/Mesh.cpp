#include "stdafx.h"
#include "Mesh.hpp"
#include "MeshPrimitive.hpp"
#include "VertexBuffer.hpp"

namespace kepler {

    Mesh::Mesh() {
    }

    Mesh::Mesh(ref<MeshPrimitive> primitive) : _primitives{primitive} {
    }

    Mesh::~Mesh() noexcept {
    }

    ref<Mesh> Mesh::create() {
        return std::make_shared<Mesh>();
    }

    ref<Mesh> Mesh::create(ref<MeshPrimitive> primitive) {
        return std::make_shared<Mesh>(primitive);
    }

    void Mesh::addMeshPrimitive(ref<MeshPrimitive> primitive) {
        _primitives.push_back(std::move(primitive));
    }

    void Mesh::setName(const std::string& name) {
        _name = std::make_unique<std::string>(name);
    }

    void Mesh::setNode(ref<Node> node) {
        for (const auto& primitive : _primitives) {
            primitive->setNode(node);
        }
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
}
