#include "stdafx.h"
#include "Mesh.hpp"
#include "MeshPrimitive.hpp"
#include "VertexBuffer.hpp"

namespace kepler {

    Mesh::Mesh() {
    }

    Mesh::Mesh(MeshPrimitiveRef primitive) : _primitives{primitive} {
    }

    Mesh::~Mesh() noexcept {
    }

    MeshRef Mesh::create() {
        return std::make_shared<Mesh>();
    }

    MeshRef Mesh::create(MeshPrimitiveRef primitive) {
        return std::make_shared<Mesh>(primitive);
    }

    void Mesh::addMeshPrimitive(MeshPrimitiveRef primitive) {
        _primitives.push_back(std::move(primitive));
    }

    void Mesh::setName(const std::string& name) {
        _name = std::make_unique<std::string>(name);
    }

    void Mesh::setNode(NodeRef node) {
        for (const auto& primitive : _primitives) {
            primitive->setNode(node);
        }
    }

    MeshPrimitiveRef Mesh::primitiveAt(size_t index) const {
        return _primitives.at(index);
    }

    MeshPrimitive* Mesh::primitivePtr(size_t index) const {
        return _primitives[index].get();
    }

    size_t Mesh::primitiveCount() const {
        return _primitives.size();
    }
}
