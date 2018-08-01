#include "stdafx.h"
#include "VulkanUtils.hpp"

namespace kepler {
namespace vulkan {

QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice physicalDevice) {
    QueueFamilyIndices indices;

    auto queueFamilies = physicalDevice.getQueueFamilyProperties();
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphicsFamily = i;
        }
        vk::Bool32 presentSupport = physicalDevice.getSurfaceSupportKHR(i, g_primarySurface);
        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.presentFamily = i;
        }
        if (indices.isComplete()) {
            break;
        }
        i++;
    }
    return indices;
}

SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice physicalDevice) {
    SwapChainSupportDetails details;
    details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(g_primarySurface);
    details.formats = physicalDevice.getSurfaceFormatsKHR(g_primarySurface);
    details.presentModes = physicalDevice.getSurfacePresentModesKHR(g_primarySurface);
    return details;
}

}
}
