#include "stdafx.h"
#include "MeshRenderer.hpp"
#include "Mesh.hpp"
#include "MeshPrimitive.hpp"

namespace kepler {
namespace gl {

static std::string _typeName("MeshRenderer");

MeshRenderer::MeshRenderer(const shared_ptr<Mesh>& mesh) : _mesh(mesh) {
}

MeshRenderer::~MeshRenderer() noexcept {
}

shared_ptr<MeshRenderer> MeshRenderer::create(const shared_ptr<Mesh>& mesh) {
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

void MeshRenderer::onNodeChanged(const shared_ptr<Node>&, const shared_ptr<Node>&) {
    if (_mesh) {
        _mesh->setNode(node());
    }
}

shared_ptr<Mesh> MeshRenderer::mesh() const {
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
}
