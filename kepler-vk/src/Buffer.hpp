#pragma once

#include <BaseVk.hpp>

namespace kepler {
namespace vulkan {

class Buffer {
public:
    using SharedPtr = std::shared_ptr<Buffer>;

    Buffer(size_t size, vk::BufferUsageFlags bufferUsage, VmaMemoryUsage vmaMemoryUsage);
    virtual ~Buffer();
    static Buffer::SharedPtr create(size_t size, vk::BufferUsageFlags bufferUsage, VmaMemoryUsage vmaMemoryUsage);

    void* map();
    void unmap();


    operator vk::Buffer() const {
        return _buffer;
    }

public:
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
protected:
    size_t _size = 0;
    vk::Buffer _buffer;
    VmaAllocation _allocation = {};
};
}
}
