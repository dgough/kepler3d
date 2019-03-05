#pragma once

#include <BaseVk.hpp>
#include <BoundingBox.hpp>
#include <Buffer.hpp>
#include <VertexLayout.hpp>
#include <Vao.hpp>
#include <vector>

namespace kepler {
namespace vulkan {

class MeshPrimitive {
    friend Mesh;
public:
    using SharedPtr = std::shared_ptr<MeshPrimitive>;

    MeshPrimitive();
    virtual ~MeshPrimitive();
    static MeshPrimitive::SharedPtr create(
        const std::vector<Buffer::SharedPtr>& vertexBuffers,
        uint32_t vertexCount,
        const Buffer::SharedPtr& indexBuffer,
        uint32_t indexCount,
        const VertexLayout::SharedPtr& vertexLayout,
        Topology topology);


    const BoundingBox& boundingBox() const;

    void setBoundingBox(const vec3& min, const vec3& max);

public:
    MeshPrimitive(const MeshPrimitive&) = delete;
    MeshPrimitive& operator=(const MeshPrimitive&) = delete;
protected:

    void setNode(const shared_ptr<Node>& node);

    std::weak_ptr<Node> _node;
    BoundingBox _box;
};
}
}
