#include "stdafx.h"
#include "Buffer.hpp"
#include <VulkanUtils.hpp>

namespace kepler {
namespace vulkan {

Buffer::Buffer(size_t size, vk::BufferUsageFlags bufferUsage, VmaMemoryUsage vmaMemoryUsage) : _size(size) {

}

Buffer::~Buffer() {
    if (_buffer) {
        vmaDestroyBuffer(g_allocator, _buffer, _allocation);
    }
}

Buffer::SharedPtr Buffer::create(size_t size, vk::BufferUsageFlags bufferUsage, VmaMemoryUsage vmaMemoryUsage) {
    auto buffer = std::make_shared<Buffer>(size, bufferUsage, vmaMemoryUsage);

    vk::BufferCreateInfo bufferInfo;
    bufferInfo.size = size;
    bufferInfo.usage = bufferUsage;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = vmaMemoryUsage;
    VkResult result = vmaCreateBuffer(g_allocator, vkPtr(bufferInfo), &allocInfo, vkPtr(buffer->_buffer), &buffer->_allocation, nullptr);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed vmaCreateBuffer");
    }
    return buffer;
}

void* Buffer::map() {
    // TODO: need to make sure that the buffer is host visible
    void* mappedData;
    VkResult result = vmaMapMemory(g_allocator, _allocation, &mappedData);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed Buffer::map");
    }
    return mappedData;
}

void Buffer::unmap() {
    // TODO: need to make sure that the buffer is host visible
    vmaUnmapMemory(g_allocator, _allocation);
}

}
}
