#include "stdafx.h"
#include "Mesh.hpp"
#include "MeshPrimitive.hpp"
#include "VertexBuffer.hpp"

namespace kepler {

    Mesh::Mesh() {
    }
    Mesh::~Mesh() noexcept {
    }

    MeshRef Mesh::create() {
        return MAKE_SHARED(Mesh);
    }

    MeshRef Mesh::create(MeshPrimitiveRef primitive) {
        auto mesh = create();
        mesh->addMeshPrimitive(primitive);
        return mesh;
    }

    void Mesh::addMeshPrimitive(MeshPrimitiveRef primitive) {
        _primitives.push_back(std::move(primitive));
    }

    void Mesh::setName(const std::string& name) {
        _name = name;
    }

    void Mesh::setNode(NodeRef node) {
        for (const auto& primitive : _primitives) {
            primitive->setNode(node);
        }
    }

    MeshPrimitiveRef Mesh::primitiveAt(size_t index) const {
        return _primitives.at(index);
    }

    MeshPrimitive * Mesh::primitivePtr(size_t index) const {
        return _primitives[index].get();
    }

    size_t Mesh::primitiveCount() const {
        return _primitives.size();
    }
}
