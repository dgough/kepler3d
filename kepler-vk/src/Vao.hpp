#pragma once

#include <BaseVk.hpp>
#include <VertexLayout.hpp>
#include <Buffer.hpp>

namespace kepler {
namespace vulkan {

enum class Topology {
    POINT_LIST = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
    LINE_LIST = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    LINE_STRIP = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
    TRIANGLE_LIST = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    TRIANGLE_STRIP = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
    TRIANGLE_FAN = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
};

class Vao {
public:
    using SharedPtr = std::shared_ptr<Vao>;

    Vao(VertexLayout::SharedPtr vertexLayout,
        std::vector<Buffer::SharedPtr> vertexBuffers,
        Buffer::SharedPtr indexBuffer,
        Topology topology);
    virtual ~Vao();
    static Vao::SharedPtr create(
        VertexLayout::SharedPtr vertexLayout,
        std::vector<Buffer::SharedPtr> vertexBuffers,
        Buffer::SharedPtr indexBuffer,
        Topology topology);

public:
    Vao(const Vao&) = delete;
    Vao& operator=(const Vao&) = delete;
protected:
    VertexLayout::SharedPtr _vertexLayout;
    std::vector<Buffer::SharedPtr> _vertexBuffers;
    Buffer::SharedPtr _indexBuffer;
    Topology _topology;
};
}
}
