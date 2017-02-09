#include "stdafx.h"
#include "MeshPrimitive.hpp"
#include "Material.hpp"
#include "Technique.hpp"
#include "Effect.hpp"
#include "VertexAttributeAccessor.hpp"
#include "VertexAttributeBinding.hpp"
#include "IndexAccessor.hpp"

namespace kepler {

    MeshPrimitive::MeshPrimitive(Mode mode) : _mode(mode) {
    }

    MeshPrimitive::~MeshPrimitive() noexcept {
    }

    ref<MeshPrimitive> MeshPrimitive::create(Mode mode) {
        return MAKE_SHARED(MeshPrimitive, mode);
    }

    ref<VertexAttributeAccessor> MeshPrimitive::attribute(AttributeSemantic semantic) const {
        auto it = _attributes.find(semantic);
        if (it != _attributes.end()) {
            return it->second;
        }
        return nullptr;
    }

    ref<IndexAccessor> MeshPrimitive::indices() const {
        return _indices;
    }

    void MeshPrimitive::setAttribute(AttributeSemantic semantic, ref<VertexAttributeAccessor> accessor) {
        _attributes[semantic] = accessor;
    }

    void MeshPrimitive::setIndices(ref<IndexAccessor> indices) {
        _indices = indices;
    }

    ref<Material> MeshPrimitive::material() const {
        return _material;
    }

    void MeshPrimitive::setMaterial(ref<Material> material) {
        if (material == nullptr) {
            // TODO clear other stuff?
            _materialBinding.release();
            return;
        }
        if (_material) {
            _materialBinding.release();
        }

        auto tech = material->technique();
        if (tech == nullptr) {
            return;
        }
        // TODO pass?

        auto effect = tech->effect();
        if (effect == nullptr) {
            return;
        }

        // create vertex binding
        // bind it to the pass?

        // TODO should this be a weak reference?
        // should attributes be in another object? A geometry?
        _material = material;
        _vertexBinding = VertexAttributeBinding::createUnique(shared_from_this(), tech);
        updateBindings();
    }

    void MeshPrimitive::draw() {
        auto node = _node.lock();
        if (_vertexBinding == nullptr || !node) {
            return;
        }
        const auto& material = *_material;
        _materialBinding->bind(*node, material);
        _vertexBinding->bind();
        if (_indices) {
            glDrawElements(_mode, _indices->count(), _indices->type(), (const GLvoid*)_indices->offset());
        }
        else {
            auto attrib = _attributes.begin();
            if (attrib != _attributes.end()) {
                glDrawArrays(_mode, 0, attrib->second->count());
            }
        }
        _vertexBinding->unbind();
    }

    void MeshPrimitive::updateBindings() {
        if (_node.expired() || _material == nullptr) {
            _materialBinding = nullptr;
            return;
        }
        if (_materialBinding == nullptr) {
            _materialBinding = std::make_unique<MaterialBinding>();
        }
        _materialBinding->updateBindings(*_material);
    }

    void MeshPrimitive::setNode(ref<Node> node) {
        _node = node;
        updateBindings();
    }
}
