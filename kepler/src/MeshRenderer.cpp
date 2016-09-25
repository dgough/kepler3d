#include "stdafx.h"
#include "MeshRenderer.hpp"
#include "Mesh.hpp"
#include "MeshPrimitive.hpp"

namespace kepler {

    static std::string _typeName("MeshRenderer");

    MeshRenderer::MeshRenderer(MeshRef mesh) : _mesh(mesh) {
    }

    MeshRenderer::~MeshRenderer() noexcept {
    }

    MeshRendererRef MeshRenderer::create(MeshRef mesh) {
        return MAKE_SHARED(MeshRenderer, mesh);
    }

    void MeshRenderer::draw() {
        if (_mesh) {
            size_t count = _mesh->primitiveCount();
            for (size_t i = 0; i < count; ++i) {
                _mesh->primitivePtr(i)->draw();
            }
        }
    }

    void MeshRenderer::onNodeChanged(const NodeRef&, const NodeRef&) {
        if (_mesh) {
            _mesh->setNode(node());
        }
    }

    MeshRef MeshRenderer::mesh() const {
        return _mesh;
    }

    const std::string& MeshRenderer::typeName() const {
        return _typeName;
    }
}
