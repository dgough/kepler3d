#pragma once

#include <BaseVk.hpp>

namespace kepler {
namespace vulkan {

class VertexBufferLayout {
public:
    using SharedPtr = std::shared_ptr<VertexBufferLayout>;
    VertexBufferLayout() = default;

    static VertexBufferLayout::SharedPtr create() {
        return std::make_shared<VertexBufferLayout>();
    }

public:
    VertexBufferLayout(const VertexBufferLayout&) = delete;
    VertexBufferLayout& operator=(const VertexBufferLayout&) = delete;
private:

};

class VertexLayout {
public:
    using SharedPtr = std::shared_ptr<VertexLayout>;

    VertexLayout();
    virtual ~VertexLayout();
    static VertexLayout::SharedPtr create();

public:
    VertexLayout(const VertexLayout&) = delete;
    VertexLayout& operator=(const VertexLayout&) = delete;
protected:

};
}
}
