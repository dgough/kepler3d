#pragma once

#include <BaseVk.hpp>

namespace kepler {
namespace vulkan {

class SwapChain {
public:
    using SharedPtr = std::shared_ptr<SwapChain>;

    SwapChain();
    virtual ~SwapChain();
    static SwapChain::SharedPtr create();

    uint32_t width() const;
    uint32_t height() const;
    const vk::Extent2D& extent() const;
    vk::Format imageFormat() const;
    const std::vector<vk::ImageView>& imageViews() const;

    operator vk::SwapchainKHR() const {
        return _swapChain;
    }

public:
    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;
private:
    void createSwapChain();
    void createImageViews();
    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes);
    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
    void cleanupSwapChain();

    vk::SwapchainKHR _swapChain;
    std::vector<vk::Image> _swapChainImages;
    vk::Format _swapChainImageFormat;
    vk::Extent2D _swapChainExtent;
    std::vector<vk::ImageView> _swapChainImageViews;
};
}
}
