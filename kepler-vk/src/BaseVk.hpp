#pragma once

#include <Base.hpp>
#include <vulkan/vulkan.hpp>
#include "vk_mem_alloc.h"

namespace kepler {
namespace vulkan {

struct QueueFamilyIndices {
    int graphicsFamily = -1;
    int presentFamily = -1;

    bool isComplete() {
        return graphicsFamily >= 0 && presentFamily >= 0;
    }
};

class VulkanState;

class Mesh;
class MeshPrimitive;
class MeshRenderer;

extern vk::Instance g_instance;
extern vk::PhysicalDevice g_physicalDevice;
extern vk::Device g_device;
extern vk::SurfaceKHR g_primarySurface;
extern VmaAllocator g_allocator;

}
}
