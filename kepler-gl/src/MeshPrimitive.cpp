#include "stdafx.h"
#include "MeshPrimitive.hpp"
#include "Material.hpp"
#include "Technique.hpp"
#include "Effect.hpp"
#include "VertexAttributeAccessor.hpp"
#include "VertexAttributeBinding.hpp"
#include "IndexAccessor.hpp"

namespace kepler {
namespace gl {

MeshPrimitive::MeshPrimitive(Mode mode) : _mode(mode) {
}

shared_ptr<MeshPrimitive> MeshPrimitive::create(Mode mode) {
    return std::make_shared<MeshPrimitive>(mode);
}

shared_ptr<VertexAttributeAccessor> MeshPrimitive::attribute(AttributeSemantic semantic) const {
    auto it = _attributes.find(semantic);
    if (it != _attributes.end()) {
        return it->second;
    }
    return nullptr;
}

bool MeshPrimitive::hasAttribute(AttributeSemantic semantic) const {
    return _attributes.count(semantic) != 0;
}

void MeshPrimitive::bindIndices() const {
    if (_indices) {
        _indices->bind();
    }
}

void MeshPrimitive::setAttribute(AttributeSemantic semantic, const shared_ptr<VertexAttributeAccessor>& accessor) {
    _attributes[semantic] = accessor;
}

void MeshPrimitive::setIndices(const shared_ptr<IndexAccessor>& indices) {
    _indices = indices;
}

shared_ptr<Material> MeshPrimitive::material() const {
    return _material;
}

void MeshPrimitive::setMaterial(const shared_ptr<Material>& material) {
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
    _vertexBinding = VertexAttributeBinding(*this, *tech, *effect);
    updateBindings();
}

const BoundingBox& MeshPrimitive::boundingBox() const {
    return _box;
}

void MeshPrimitive::setBoundingBox(const vec3& min, const vec3& max) {
    _box.set(min, max);
}

void MeshPrimitive::draw() {
    auto node = _node.lock();
    if (!_vertexBinding || !node) {
        return;
    }
    const auto& material = *_material;
    _materialBinding->bind(*node, material);
    _vertexBinding.bind();
    if (_indices) {
        glDrawElements(_mode, _indices->count(), _indices->type(), (const GLvoid*)_indices->offset());
    }
    else {
        auto attrib = _attributes.begin();
        if (attrib != _attributes.end()) {
            glDrawArrays(_mode, 0, attrib->second->count());
        }
    }
    _vertexBinding.unbind();
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

void MeshPrimitive::setNode(const shared_ptr<Node>& node) {
    _node = node;
    updateBindings();
}
}
}
