#include "stdafx.h"
#include "Vao.hpp"

namespace kepler {
namespace vulkan {

Vao::Vao(VertexLayout::SharedPtr vertexLayout,
    std::vector<Buffer::SharedPtr> vertexBuffers,
    Buffer::SharedPtr indexBuffer,
    Topology topology) :
    _vertexLayout(vertexLayout), _vertexBuffers(vertexBuffers), _indexBuffer(indexBuffer), _topology(topology) {
}

Vao::~Vao() {
}

Vao::SharedPtr Vao::create(VertexLayout::SharedPtr vertexLayout, std::vector<Buffer::SharedPtr> vertexBuffers,
    Buffer::SharedPtr indexBuffer, Topology topology) {
    return std::make_shared<Vao>(vertexLayout, vertexBuffers, indexBuffer, topology);
}

}
}
