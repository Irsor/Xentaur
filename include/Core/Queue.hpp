#pragma once

#include <iostream>

#include <vulkan/vulkan.hpp>

namespace xe_core {
    class Queue {
    public:
        Queue(const vk::UniqueDevice &vulkanDevice, const vk::UniqueSwapchainKHR &vulkanSwapchain, int queueFamily, int queueIndex);
        ~Queue();

        void Init();

        int AcquireNextImage();
        void Submit(const vk::UniqueCommandBuffer&commandBuffer);
        void SubmitAsync(const vk::UniqueCommandBuffer &commandBuffer);
        void Present(const unsigned int imageIndex);
        void Wait();

    private:
        void CreateSemaphores();

        vk::Device device;
        vk::SwapchainKHR swapchain;
        vk::Queue queue;
        vk::UniqueSemaphore renderCompleteSemaphore;
        vk::UniqueSemaphore presentCompleteSemaphore;
    };
}