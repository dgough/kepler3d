#pragma once

#include <vulkan/vulkan.hpp>
#include <BaseVk.hpp>

namespace kepler {
namespace vulkan {

struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};


QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice physicalDevice);
SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice physicalDevice);

inline VkBuffer* vkPtr(vk::Buffer& buffer) {
    return reinterpret_cast<VkBuffer*>(&buffer);
}

inline VkBufferCreateInfo* vkPtr(vk::BufferCreateInfo& info) {
    return reinterpret_cast<VkBufferCreateInfo*>(&info);
}

}
}
