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
        for (auto primitive : _primitives) {
            primitive->setNode(node);
        }
    }

    MeshPrimitiveRef Mesh::getPrimitive(size_t index) const {
        return _primitives.at(index);
    }

    size_t Mesh::getPrimitiveCount() const {
        return _primitives.size();
    }
}
