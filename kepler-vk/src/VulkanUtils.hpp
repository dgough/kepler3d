#pragma once

#include <vulkan/vulkan.hpp>

namespace kepler {
namespace vulkan {

inline VkBuffer* vkPtr(vk::Buffer& buffer) {
    return reinterpret_cast<VkBuffer*>(&buffer);
}

inline VkBufferCreateInfo* vkPtr(vk::BufferCreateInfo& info) {
    return reinterpret_cast<VkBufferCreateInfo*>(&info);
}


}
}
