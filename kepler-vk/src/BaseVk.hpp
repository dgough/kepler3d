#pragma once

#include <Base.hpp>
#include <vulkan/vulkan.hpp>
#include "vk_mem_alloc.h"

#include <VulkanUtils.hpp>

namespace kepler {
namespace vulkan {

class VulkanState;

class Mesh;
class MeshPrimitive;
class MeshRenderer;

extern vk::Instance g_instance;
extern vk::Device g_device;
extern VmaAllocator g_allocator;

}
}
