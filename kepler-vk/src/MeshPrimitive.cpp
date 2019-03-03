#include "stdafx.h"
#include "MeshPrimitive.hpp"

namespace kepler {
namespace vulkan {

MeshPrimitive::MeshPrimitive() {
}

MeshPrimitive::~MeshPrimitive() {
}

MeshPrimitive::SharedPtr MeshPrimitive::create(
    const std::vector<Buffer::SharedPtr>& vertexBuffers,
    uint32_t vertexCount,
    const Buffer::SharedPtr& indexBuffer,
    uint32_t indexCount,
    const VertexLayout::SharedPtr& vertexLayout,
    Topology topology) {
    //auto ptr = std::make_shared<MeshPrimitive>();
    return nullptr;
}



const BoundingBox& MeshPrimitive::boundingBox() const {
    return _box;
}

void MeshPrimitive::setBoundingBox(const vec3& min, const vec3& max) {
    _box.set(min, max);
}

void MeshPrimitive::setNode(const shared_ptr<Node>& node) {
    _node = node;
    //updateBindings();
}

}
}
