#pragma once

#include <set>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <BaseVk.hpp>
#include <SwapChain.hpp>
#include <Buffer.hpp>

namespace kepler {
namespace vulkan {

class VulkanState {
public:

    VulkanState(GLFWwindow* window);
    void deviceWaitIdle();

    void initVulkan();
    void cleanupSwapChain();
    void cleanup();
    void recreateSwapChain();
    void createInstance();
    void setupDebugCallback();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffer();
    void createDescriptorPool();
    void createDescriptorSet();
    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);
    void copyBuffer(Buffer& srcBuffer, Buffer& dstBuffer, vk::DeviceSize size);
    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    void createCommandBuffers();
    void createSyncObjects();
    void updateUniformBuffer();
    void drawFrame();
    vk::ShaderModule createShaderModule(const std::vector<char>& code);
    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes);
    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
    bool isDeviceSuitable(vk::PhysicalDevice device);
    bool checkDeviceExtensionSupport(vk::PhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();

private:

    void createVma();

    GLFWwindow* _window;

    vk::DispatchLoaderDynamic _instanceDispatchLoader;
    vk::DebugReportCallbackEXT _callback;

    vk::Queue _graphicsQueue;
    vk::Queue _presentQueue;

    SwapChain::SharedPtr _swapChain;

    std::vector<vk::Framebuffer> _swapChainFramebuffers;
    vk::RenderPass _renderPass;
    vk::DescriptorSetLayout _descriptorSetLayout;
    vk::PipelineLayout _pipelineLayout;
    vk::Pipeline _graphicsPipeline;

    vk::CommandPool _commandPool;

    Buffer::SharedPtr _vertexBuffer;
    Buffer::SharedPtr _indexBuffer;
    Buffer::SharedPtr _uniformBuffer;

    vk::DescriptorPool _descriptorPool;
    vk::DescriptorSet _descriptorSet;

    std::vector<vk::CommandBuffer> _commandBuffers;

    std::vector<vk::Semaphore> _imageAvailableSemaphores;
    std::vector<vk::Semaphore> _renderFinishedSemaphores;
    std::vector<vk::Fence> _inFlightFences;

    size_t _currentFrame = 0;
};

}
}
