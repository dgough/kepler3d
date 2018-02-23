#include "stdafx.h"
#include "MeshRenderer.hpp"
#include "Mesh.hpp"
#include "MeshPrimitive.hpp"

namespace kepler {

static std::string _typeName("MeshRenderer");

MeshRenderer::MeshRenderer(ref<Mesh> mesh) : _mesh(mesh) {
}

MeshRenderer::~MeshRenderer() noexcept {
}

ref<MeshRenderer> MeshRenderer::create(ref<Mesh> mesh) {
    return std::make_shared<MeshRenderer>(mesh);
}

void MeshRenderer::draw() {
    if (_mesh) {
        size_t count = _mesh->primitiveCount();
        for (size_t i = 0; i < count; ++i) {
            _mesh->primitivePtr(i)->draw();
        }
    }
}

void MeshRenderer::onNodeChanged(const ref<Node>&, const ref<Node>&) {
    if (_mesh) {
        _mesh->setNode(node());
    }
}

ref<Mesh> MeshRenderer::mesh() const {
    return _mesh;
}

bool MeshRenderer::getBoundingBox(BoundingBox& box) {
    if (_mesh) {
        box = _mesh->boundingBox();
        return true;
    }
    return false;
}

const std::string& MeshRenderer::typeName() const {
    return _typeName;
}
}
