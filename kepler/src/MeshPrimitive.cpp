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

    MeshPrimitiveRef MeshPrimitive::create(Mode mode) {
        return MAKE_SHARED(MeshPrimitive, mode);
    }

    VertexAttributeAccessorRef MeshPrimitive::getAttribute(AttributeSemantic semantic) const {
        auto it = _attributes.find(semantic);
        if (it != _attributes.end()) {
            return it->second;
        }
        return nullptr;
    }

    IndexAccessorRef MeshPrimitive::getIndices() const {
        return _indices;
    }

    void MeshPrimitive::setAttribute(AttributeSemantic semantic, VertexAttributeAccessorRef accessor) {
        _attributes[semantic] = accessor;
    }

    void MeshPrimitive::setIndices(IndexAccessorRef indices) {
        _indices = indices;
    }

    MaterialRef MeshPrimitive::getMaterial() const {
        return _material;
    }

    void MeshPrimitive::setMaterial(MaterialRef material) {
        if (material == nullptr) {
            // TODO clear other stuff?
            _materialBinding.release();
            return;
        }
        if (_material) {
            _materialBinding.release();
        }

        auto tech = material->getTechnique();
        if (tech == nullptr) {
            return;
        }
        // TODO pass?

        auto effect = tech->getEffect();
        if (effect == nullptr) {
            return;
        }

        // create vertex binding
        // bind it to the pass?

        // TODO should this be a weak reference?
        // should attributes be in another object? A geometry?
        _material = material;
        auto binding = VertexAttributeBinding::create(shared_from_this(), tech);
        if (binding) {
            _vertexBinding = binding;
        }

        updateBindings();
    }

    void MeshPrimitive::draw() {
        if (_vertexBinding == nullptr) {
            return;
        }
        _materialBinding->bind();
        _vertexBinding->bind();        
        if (_indices) {
            glDrawElements(_mode, _indices->getCount(), _indices->getType(), (const GLvoid*)_indices->getOffset());
        }
        else {
            auto attrib = _attributes.begin();
            if (attrib != _attributes.end()) {
                glDrawArrays(_mode, 0, attrib->second->getCount());
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
            _materialBinding = std::make_unique<MaterialBinding>(_node, _material);
        }
        _materialBinding->updateBindings(_node.lock());
    }

    void MeshPrimitive::setNode(NodeRef node) {
        _node = node;
        updateBindings();
    }
}
