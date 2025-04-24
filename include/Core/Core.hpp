#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>

#include "../Window.hpp"
#include "PhysicalDevices.hpp"
#include "Queue.hpp"

#ifndef NDEBUG
    const bool enableValidationLayers = true;
#else
    const bool enableValidationLayers = false;
#endif

namespace xe_core {
    class Core {
    public:
        Core(const std::string &appName, std::shared_ptr<xe::Window> window);
        ~Core();
        unsigned int GetNumImages() const;
        vk::Image GetImage(unsigned int index) const;
        void CreateCommandBuffers(unsigned int numImages, std::vector<vk::CommandBuffer> &buffers);
        std::shared_ptr<Queue> GetQueue();
        vk::RenderPass CreateDefaultRenderPass();
        void DestroyRenderPass(const vk::RenderPass& renderPass) const;
        std::vector<vk::Framebuffer> CreateFrameBuffers(const vk::RenderPass &renderPass);

    private:
        void Init(std::shared_ptr<xe::Window> win);
        void CreateInstance();
        std::vector<const char*> GetExtensions() const;
        void CreateSurface(std::shared_ptr<xe::Window> window);
        void GetPhysicalDevices();
        void CreateDevice();
        void CreateSwapchain();
        void CreateCommandPool();
        std::vector<vk::CommandBuffer> AllocateCommandBuffers(unsigned int count);

        std::string name;
        vk::UniqueInstance instance;
        vk::UniqueSurfaceKHR surface;
        std::unique_ptr<PhysicalDevices> physicalDevices;
        unsigned int queueFamily;
        vk::UniqueDevice device;
        vk::UniqueSwapchainKHR swapchain;
        std::vector<vk::Image> images{};
        std::vector<vk::ImageView> imageViews{};
        vk::UniqueCommandPool commandPool;
        std::shared_ptr<Queue> queue;
        vk::SurfaceFormatKHR surfaceFormat{};
        std::vector<vk::Framebuffer> frameBuffers{};
        std::shared_ptr<xe::Window> window;
    };
}