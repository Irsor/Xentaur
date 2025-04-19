#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>

#include "../Window.hpp"

#ifndef NDEBUG
    const bool enableValidationLayers = true;
#else
    const bool enableValidationLayers = false;
#endif

namespace xe_core {
    class VulkanCore {
    public:
        VulkanCore(const std::string &appName, std::shared_ptr<xe::Window> window);
        ~VulkanCore();

    private:
        void Init(std::shared_ptr<xe::Window> window);
        void CreateInstance();
        std::vector<const char*> GetExtensions() const;
        void CreateSurface(std::shared_ptr<xe::Window> window);

        std::string name;
        vk::UniqueInstance instance;
        vk::UniqueSurfaceKHR surface;
    };
}