#include "stdafx.h"
#include "MeshRenderer.hpp"
#include "Mesh.hpp"
#include "MeshPrimitive.hpp"

namespace kepler {

    static std::string typeName("MeshRenderer");

    MeshRenderer::MeshRenderer(MeshRef mesh) : _mesh(mesh) {
    }

    MeshRenderer::~MeshRenderer() noexcept {
    }

    MeshRendererRef MeshRenderer::create(MeshRef mesh) {
        return MAKE_SHARED(MeshRenderer, mesh);
    }

    void MeshRenderer::draw() {
        if (_mesh) {
            for (size_t i = 0; i < _mesh->getPrimitiveCount(); ++i) {
                _mesh->getPrimitive(i)->draw();
            }
        }
    }

    void MeshRenderer::onNodeChanged(const NodeRef&, const NodeRef&) {
        if (_mesh) {
            _mesh->setNode(getNode());
        }
    }

    MeshRef MeshRenderer::getMesh() const {
        return _mesh;
    }

    const std::string& MeshRenderer::getTypeName() const {
        return typeName;
    }
}
